#ifndef __CONNECTION_H
#define __CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum connection_type {
	CONNECTION_TYPE_CLIENT,
	CONNECTION_TYPE_SERVER
} connection_type_e;

/**
 * Encapsulates the properties of a connection.
 */
typedef struct connection {
	// file descriptor of the socket that connected
	// to our server after being `accept`ed.
	int fd;

	// type of the connection represented
	connection_type_e type;
} connection_t;

/**
 * Instantiates a new connection struct
 * with a predefined `client_fd` initialized
 * value.
 */
connection_t*
connection_create(connection_type_e type);
/**
 * Closes the underlying connection file descriptor
 * and destroys the memory allocated to the conneciton
 * struct.
 */
int
connection_destroy(connection_t* conn);

/**
 * A connection handler that is called back whenever a
 * new client gets connected.
 */
typedef int (*connection_handler)(connection_t*);

#endif
