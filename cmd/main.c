#include "../src/server.h"

const char* http_response = "HTTP/1.1 200 Ok\r\n"
                            "Content-Length: 4\r\n"
                            "\r\n"
                            "OK\r\n";

size_t http_response_len = 42;

// TODO fix the blocking case - it should be able to read too
int
http_handler(connection_t* conn)
{
	int  n = 0;
	char buf[1024];

	for (;;) {
		n = read(conn->client_fd, buf, 1024);
		if (n == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}

			perror("read");
			printf("failed to read from the client\n");
			return -1;
		}

		if (n == 0) {
			break;
		}
	}

	n = write(conn->client_fd, http_response, http_response_len);
	if (n == -1) {
		perror("write");
		printf("failed to write to client\n");
		return -1;
	}

	return 0;
}

server_t server = {
	.connection_handler = &http_handler,
};

/**
 * Main server routine.
 *
 *      -       instantiates a new server structure that holds the
 *              properties of our server;
 *      -       creates a socket and makes it passive with
 *              `server_listen`;
 *      -       accepts new connections on the server socket.
 *
 */
int
main()
{
	int err = 0;

	err = server_listen(&server);
	if (err) {
		printf("Failed to listen on address :8080\n");
		return err;
	}

	err = server_serve(&server);
	if (err) {
		printf("Failed serving\n");
		return err;
	}

	return 0;
}
