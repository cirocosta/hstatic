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
	// epoll_fd is the epoll file descriptor retrieved from
	// an `epoll_creaet` op.
	int epoll_fd;

	// server connection that holds the underlying passive
	// socket where connections get accepted from.
	connection_t* conn;

	// callback to execute whenever a new connection
	// is accepted.
	connection_handler connection_handler;
} server_t;

/**
 * Destroys the instantiated server, freeing all of its
 * resources and closing and pending connections.
 */
int
server_destroy(server_t* server);

/**
 * Instantiates a new server
 */
server_t*
server_create(connection_handler handler);

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
