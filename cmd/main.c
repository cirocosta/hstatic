#include "../src/http.h"
#include "../src/server.h"

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

	server_t* server = server_create(&http_handler);

	if (server == NULL) {
		printf("failed to instantiate server\n");
		return 1;
	}

	err = server_listen(server);
	if (err) {
		printf("Failed to listen on address :8080\n");
		return err;
	}

	err = server_serve(server);
	if (err) {
		printf("Failed serving\n");
		return err;
	}

	err = server_destroy(server);
	if (err) {
		printf("failed to destroy server\n");
		return 1;
	}

	return 0;
}
