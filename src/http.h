#ifndef __HTTP_H
#define __HTTP_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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
	http_error_e error;
	http_method_e method;
	char* path;
	size_t path_len;
} http_request_t;

/**
 * Parses an HTTP request.
 *
 * @param dst   struct that will hold the informations
 *              regarding the http request
 *
 * @param req   bytes from the request to parse
 * @param n     size of the `req` line.
 * @return      whether it succeeded or not.
 */
int
http_parse_request(http_request_t* dst, char req[], size_t n);

#endif
