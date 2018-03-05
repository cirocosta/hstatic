#include "./server.h"

/**
 * Turns a socket into non-blocking by changing
 * its flags using `fcntl`.
 */
int
_make_socket_nonblocking(int socket)
{
	int err = 0;
	int flags;

	err = (flags = fcntl(socket, F_GETFL, 0));
	if (err == -1) {
		perror("fcntl");
		printf("failed to retrieve socket flags\n");
		return -1;
	}

	flags |= O_NONBLOCK;

	err = fcntl(socket, F_SETFL, flags);
	if (err == -1) {
		perror("fcntl");
		printf("failed to set socket flags\n");
		return -1;
	}

	return 0;
}

int
server_serve_non_blocking(server_t* server)
{
	int epoll_fd;
	int err = 0;

	struct epoll_event event = { 0 };
	struct epoll_event events[HSTATIC_EPOLL_EVENTS];

	// creates a new epoll instance and returns a file
	// descriptor referring to that instance.
	err = (epoll_fd = epoll_create1(0));
	if (err == -1) {
		perror("epoll_create1");
		printf("couldn't create epoll fd\n");
		return err;
	}

	// Interest in particular file descriptors is then
	// registered via epoll_ctl(2) - adds the file descriptor to
	// the epoll set.
	//
	// Here we register the target file descriptor server->listen_fd
	// on the epoll instance referred to by the file descriptor
	// epoll_fd and associate the event `event` with the internal file
	// linked to epoll_fd.
	event.data.fd = server->listen_fd;
	event.events  = EPOLLIN | EPOLLET;

	err = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->listen_fd, &event);
	if (err == -1) {
		perror("epoll_ctl");
		printf("failed to add listen socket to epoll event");
		return err;
	}

	for (;;) {
		int fds_len;

		// Wait indefintely (-1) until there's a file descriptor ready
		// to proceed with IO in a non-blocking manner.
		//
		// When at least one file descriptor is ready, we'll receive in
		// `fds_len` the number of file descriptors ready.
		//
		// `events` array gets populated with the events, which allows
		// us
		// to retrieve these events by simply looping over the array.
		fds_len =
		  epoll_wait(epoll_fd, events, HSTATIC_EPOLL_EVENTS, -1);
		if (fds_len == -1) {
			perror("epoll_wait");
			printf("failed to wait for epoll events");
			// TODO close epoll_fd?
			return -1;
		}

		for (int i = 0; i < fds_len; i++) {
			if ((events[i].events & EPOLLERR) ||
			    (events[i].events & EPOLLHUP) ||
			    (!(events[i].events & EPOLLIN))) {
				printf("Unexpected epoll error\n");
				// TODO check if this `close` will remove the fd
				//      from the epoll set
				close(events[i].data.fd);
				continue;
			}

			// If we're getting a notification of IO ready in our
			// server listener fd, then that means we have at least
			// one new connection waiting to be accepted.
			//
			// To make sure we accept them all, try to accept as
			// much
			// as we can until an EAGAIN or EWOULDBLOCK is reached.
			if (events[i].data.fd == server->listen_fd) {
				for (;;) {

					struct sockaddr in_addr;
					socklen_t       in_len;
					int             infd;

					in_len = sizeof in_addr;
					infd   = accept(
					  server->listen_fd, &in_addr, &in_len);
					if (infd == -1) {
						// All incoming
						// connections
						// have been processed.
						if ((errno == EAGAIN) ||
						    (errno == EWOULDBLOCK)) {
							break;
						}

						perror("accept");
						printf("failed unexpectedly "
						       "while accepting "
						       "connection");
						return -1;
					}

					// Make the incoming socket non-blocking
					// and add it to the list of fds to
					// monitor.
					_make_socket_nonblocking(infd);

					event.data.fd = infd;
					event.events  = EPOLLIN | EPOLLET;
					err           = epoll_ctl(epoll_fd,
					                EPOLL_CTL_ADD,
					                infd,
					                &event);
					if (err == -1) {
						perror("epoll_ctl");
						printf("couldn't add client "
						       "socket to epoll set\n");
						return -1;
					}
				}
			}
		}
	}

	return 0;
}

int
server_serve(server_t* server)
{
	int err = 0;

	for (;;) {
		err = server_accept(server);
		if (err) {
			printf("failed accepting connection\n");
			return err;
		}
	}

	return err;
}

int
server_accept(server_t* server)
{
	int                err = 0;
	int                conn_fd;
	socklen_t          client_len;
	struct sockaddr_in client_addr;

	client_len = sizeof(client_addr);

	// accept(2) extracts the first connection request on the queue
	// of pending connections for the listening socket, sockfd,
	// creates a new connected socket, and returns a new file descriptor
	// referring to that socket.
	//
	// ps.:  the original socket sockfd is unaffected by this call.
	err =
	  (conn_fd = accept(
	     server->listen_fd, (struct sockaddr*)&client_addr, &client_len));
	if (err == -1) {
		perror("accept");
		printf("failed accepting connection\n");
		return err;
	}

	connection_t conn = {
		.client_fd = conn_fd,
	};

	if (server->connection_handler == NULL) {
		err = -1;
		printf("connection_handler must be specified\n");
		return err;
	}

	err = server->connection_handler(&conn);
	if (err) {
		printf("failed to handle connection\n");
		return err;
	}

	err = close(conn_fd);
	if (err == -1) {
		perror("close");
		printf("failed to close connection\n");
		return err;
	}

	return err;
}

int
server_listen(server_t* server)
{
	int                err         = 0;
	struct sockaddr_in server_addr = { 0 };

	// `sockaddr_in` provides ways of representing a full address
	// composed of an IP address and a port.
	//
	// SIN_FAMILY   address family          AF_INET refers to the
	//                                      address family related to
	//                                      internet addresses
	//
	// S_ADDR       address (ip) in network byte order (big endian)
	// SIN_PORT     port in network byte order (big endian)
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(HSTATIC_PORT);

	// The `socket(2)` syscall creates an endpoint for communication
	// and returns a file descriptor that refers to that endpoint.
	//
	// It takes three arguments (the last being just to provide
	// greater specificity):
	// -    domain (communication domain)
	//      AF_INET              IPv4 Internet protocols
	//
	// -    type (communication semantics)
	//      SOCK_STREAM          Provides sequenced, reliable,
	//                           two-way, connection-based byte
	//                           streams.
	err = (server->listen_fd = socket(AF_INET, SOCK_STREAM, 0));
	if (err == -1) {
		perror("socket");
		printf("Failed to create socket endpoint\n");
		return err;
	}

	// bind() assigns the address specified to the socket referred
	// to by the file descriptor (`listen_fd`).
	//
	// Here we cast `sockaddr_in` to `sockaddr` and specify the
	// length such that `bind` can pick the values from the
	// right offsets when interpreting the structure pointed to.
	err = bind(server->listen_fd,
	           (struct sockaddr*)&server_addr,
	           sizeof(server_addr));
	if (err == -1) {
		perror("bind");
		printf("Failed to bind socket to address\n");
		return err;
	}

	if (server->non_blocking) {
		// Makes the server socket non-blocking such that calls that
		// would block return a -1 with EAGAIN or EWOULDBLOCK and
		// return immediately.
		err = _make_socket_nonblocking(server->listen_fd);
		if (err) {
			printf("failed to make server socket nonblocking\n");
			return err;
		}
	}

	// listen() marks the socket referred to by sockfd as a
	// passive socket, that is, as a socket that will be used to accept
	// incoming connection requests using accept(2).
	err = listen(server->listen_fd, HSTATIC_TCP_BACKLOG);
	if (err == -1) {
		perror("listen");
		printf("Failed to put socket in passive mode\n");
		return err;
	}

	return 0;
}
