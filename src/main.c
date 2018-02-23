#include "./server.h"

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
	server_t server = { 0 };

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
