#include "../src/http.h"
#include "../src/server.h"

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
