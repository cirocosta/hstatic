#include "./server.h"

const char* http_response = "HTTP/1.1 200 Ok\r\n"
                            "Content-Length: 2\r\n"
                            "\r\n"
                            "OK\r\n";

size_t http_response_len = 42;

int
http_handler(connection_t* conn)
{
	int n = 0;

	n = write(conn->client_fd, http_response, http_response_len);
	if (n == -1) {
		perror("write");
		printf("failed to write to client\n");
		return -1;
	}

	return 0;
}

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
	server_t server = {
		.connection_handler = &http_handler,
	};

	err = server_listen(&server);
	if (err) {
		printf("Failed to listen on address :8080\n");
		return err;
	}

	for (;;) {
		err = server_accept(&server);
		if (err) {
			printf("Failed accepting connection\n");
			return err;
		}
	}

	return 0;
}
