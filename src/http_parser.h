#ifndef __SRC_HTTP_PARSER_H
#define __SRC_HTTP_PARSER_H

#include "./connection.h"
#include "./http.h"

#define HSTATIC_HTTP_PARSER_BUFFER_SIZE 4096

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

	//  Buffer buf_len
	size_t buf_len;

	// Buffer - contains the bytes read so far.
	char buf[HSTATIC_HTTP_PARSER_BUFFER_SIZE];
} http_parser_t;

/**
 * Instantiates a new HTTP parser.
 */
http_parser_t*
http_parser_create(connection_t* conn);

/**
 * Tries to fill the http_parser_t buffer by reading as much as it
 * can from the connection to the internal buffer without blocking.
 *
 * ps.: not being able to read (EWOULDBLOCK or EAGAIN) is not seen
 *      as an error (0 is returned).
 */
int
http_parser_try_fill_buffer(http_parser_t* parser);

/**
 * Retrieves a token from the connection and puts into the
 * parser state machine.
 *
 * For instance, consider the following request:
 *
 *      GET / HTTP/1.1\r\n
 *
 * If we truncate this into streams of 3 bytes each, we get:
 *
 *       +-------------+
 *      1|  G   E   T  |
 *      2|      /      |
 *      3|  H   T   T  |
 *      4|  P   /   1  |
 *      5|  .   1   \r |
 *      6|  \n         |
 *       +-------------+
 *
 *  The tokenizer operates by inspecting the source buffer
 *  (http_parser->buf) and going all the way up to `len`
 *  looking for the first end of token (space).
 *
 *  If it doesn't find one, then it toggles a flag that
 *  tells the reader to provide more input to it.
 *
 *  Reading 3 bytes at each time, we'd have:
 *
 *  call #      description
 *  ------      -------------------------------------------
 *  1.      ==> reads the parser buf until it reaches
 *              the max length - sets need_more (didn't find a
 *              space yet);
 *
 *  2.      ==> finds a space: what's been read until now
 *              is a token - marks it under `tokens`.
 *
 *  3.      ==> repeat ...
 */
int
http_parser_try_get_token(http_parser_t* parser);

#endif
