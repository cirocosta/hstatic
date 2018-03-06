#include "./connection.h"

connection_t*
connection_create(connection_type_e type)
{
	connection_t* conn = malloc(sizeof(*conn));

	if (conn == NULL) {
		perror("malloc");
		printf("failed to allocate memory for connection\n");
		return conn;
	}

	conn->type = type;
	conn->fd   = -1;

	return conn;
}

int
connection_destroy(connection_t* conn)
{
	int err;

	if (conn == NULL) {
		return 0;
	}

	if (conn->fd != -1) {
		err = close(conn->fd);
		if (err == -1) {
			perror("close");
			printf("failed to close connection file descriptor\n");
			return -1;
		}
	}

	free(conn);
	return 0;
}
