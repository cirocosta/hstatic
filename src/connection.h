#ifndef __CONNECTION_H
#define __CONNECTION_H

#include <unistd.h>

/**
 * Encapsulates the properties of a connection.
 */
typedef struct connection {
	// file descriptor of the socket that connected
	// to our server after being `accept`ed.
	int client_fd;
} connection_t;

/**
 * A connection handler that is called back whenever a
 * new client gets connected.
 */
typedef int (*connection_handler)(connection_t*);

#endif
