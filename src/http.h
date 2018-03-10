#ifndef __SRC_HTTP_H
#define __SRC_HTTP_H

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "./connection.h"

typedef enum http_error {
	HTTP_ERROR_NONE,
	HTTP_ERROR_BAD_REQUEST,
} http_error_e;

/**
 * Possible HTTP request methods.
 */
typedef enum http_method {
	HTTP_METHOD_UNKNOWN,
	HTTP_METHOD_GET,
} http_method_e;

/**
 * Represents a parsed HTTP request.
 */
typedef struct http_request {
	http_error_e  error;
	http_method_e method;
	char*         path;
	size_t        path_len;
} http_request_t;

/**
 * Handles incoming connections, parsing the content of
 * the requests as HTTP messages.
 */
int
http_handler(connection_t* conn);

#endif
