#ifndef __HTTP_H
#define __HTTP_H

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
 *  Represents the state in which the parsing is at
 *  the moment.
 *
 *      Request       =    Request-Line              ;
 *                         *(( general-header        ;
 *                          | request-header         ;
 *                          | entity-header ) CRLF)  ;
 *                         CRLF
 *                         [ message-body ]          ;
 *
 *      Request-Line   =   Method               SP
 *                         Request-URI          SP
 *                         HTTP-Version CRLF
 *
 *      message-header =   field-name ":" [ field-value ]
 *
 */
typedef enum http_parser_state {
	HTTP_PARSER_STATE_NEW,
	HTTP_PARSER_STATE_REQUEST_LINE_METHOD,
	HTTP_PARSER_STATE_REQUEST_LINE_PATH,
	HTTP_PARSER_STATE_REQUEST_LINE_PROTOCOL,
	HTTP_PARSER_STATE_DONE,
} http_parser_state_e;

/**
 * Keeps track of the state of the parsing machine for
 * a given request.
 */
typedef struct http_parser {
	// The connection that is providing the contents
	// that fill the buffer
	connection_t* conn;

	// Describes in which state the parser is
	http_parser_state_e state;

	// The parsed HTTP request up to the moment
	http_request_t req;

	// Buffer - contains the bytes read so far.
	char buf[4096];

	// Keeps track of the last 3 tokens found
	char* tokens[3];
} http_parser_t;

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

/**
 * Handles incoming connections, parsing the content of
 * the requests as HTTP messages.
 */
int
http_handler(connection_t* conn);

#endif
