#ifndef __SERVER_H
#define __SERVER_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/socket.h>

#include "./connection.h"

/**
 * Custom configuration parameters.
 *
 * Check the Makefile at the root of the
 * repository to know more.
 */
#define HSTATIC_EPOLL_EVENTS 64
#define HSTATIC_PORT 8080
#define HSTATIC_TCP_BACKLOG 4

/**
 * Encapsulates the properties of the server.
 */
typedef struct server {
	// Defines whether the server is meant to perform
	// all operations in a non-blocking manner.
	bool non_blocking;

	// file descriptor of the socket in passive
	// mode to wait for connections.
	int listen_fd;

	// callback to execute whenever a new connection
	// is accepted.
	connection_handler connection_handler;
} server_t;

/**
 * Accepts new connections and then prints `Hello World` to
 * them.
 *
 * Given that a `server_t` has already been initialized,
 * it accepts a connection by `accept`ing on that socket.
 *
 * Note.: this method blocks until a single client is connected.
 */
int
server_accept(server_t* server);

/**
 * Accepts connections and processes them using the handler specfied
 * in the server struct.
 */
int
server_serve(server_t* server);

/**
 * Creates a socket for the server and makes it passive such that
 * we can wait for connections on it later.
 *
 * It uses `INADDR_ANY` (0.0.0.0) to bind to all the interfaces
 * available.
 *
 * The port is defined at compile time via the PORT definition.
 */
int
server_listen(server_t* server);

#endif
