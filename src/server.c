#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

#define PORT 8080
#define BACKLOG 128

int
listen_for_connections()
{
	int err = 0;
	int listen_fd;
	struct sockaddr_in server_addr = { 0 };

	// `sockaddr_in` provides ways of representing a full address
	// composed of an IP address and a port.
	//
	// SIN_FAMILY   address family          AF_INET refers to the address
	//                                      family related to internet
	//                                      addresses
	//
	// S_ADDR       address (ip) in network byte order (big endian)
	// SIN_PORT     port in network byte order (big endian)
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	// The `socket(2)` syscall creates an endpoint for communication
	// and returns a file descriptor that refers to that endpoint.
	//
	// It takes three arguments (the last being just to provide greater
	// specificity):
	// -    domain (communication domain)
	//      AF_INET              IPv4 Internet protocols
	//
	// -    type (communication semantics)
	//      SOCK_STREAM          Provides sequenced, reliable,
	//                           two-way, connection-based byte
	//                           streams.
	err = (listen_fd = socket(AF_INET, SOCK_STREAM, 0));
	if (err == -1) {
		perror("socket");
		printf("Failed to create socket endpoint\n");
		return err;
	}

	// bind() assigns the address specified to the socket referred to by
	// the file descriptor (`listen_fd`).
	err =
	  bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (err == -1) {
		perror("bind");
		printf("Failed to bind socket to address\n");
		return err;
	}

	err = listen(listen_fd, BACKLOG);
	if (err == -1) {
		perror("listen");
		printf("Failed to put socket in passive mode\n");
		return err;
	}

	for (;;) {
		int conn_fd;
		socklen_t client_len;
		struct sockaddr_in client_addr;

		client_len = sizeof(client_addr);

		err =
		  (conn_fd = accept(
		     listen_fd, (struct sockaddr*)&client_addr, &client_len));
		if (err == -1) {
			perror("accept");
			printf("failed accepting connection\n");
			return err;
		}

		printf("Client connected!\n");

		err = close(conn_fd);
		if (err == -1) {
			perror("close");
			printf("failed to close connection\n");
			return err;
		}
	}

	return 0;
}

int
main()
{
	int err = 0;

	err = listen_for_connections();
	if (err) {
		printf("Failed to listen on address :8080\n");
		return err;
	}

	return 0;
}
