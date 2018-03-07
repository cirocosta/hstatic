#include "./http.h"

const static char* http_response = "HTTP/1.1 200 Ok\r\n"
                                   "Content-Length: 4\r\n"
                                   "\r\n"
                                   "OK\r\n";

const static size_t http_response_len = 42;

int
http_handler(connection_t* conn)
{
	int  n = 0;
	char buf[1024];

	for (;;) {
		n = read(conn->fd, buf, 1024);
		if (n == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}

			perror("read");
			printf("failed to read from the client\n");
			return -1;
		}

		if (n == 0) {
			break;
		}
	}

	n = write(conn->fd, http_response, http_response_len);
	if (n == -1) {
		perror("write");
		printf("failed to write to client\n");
		return -1;
	}

	return 0;
}

int
http_parse_request(http_request_t* dst, char req[], size_t n)
{
	size_t ndx_start = 0;
	size_t ndx_end   = 0;

	if (!n) {
		dst->error = HTTP_ERROR_BAD_REQUEST;
		return 1;
	}

	// GET /path HTTP/1.1\r\n

	while (ndx_end < n) {
		if (isspace(req[ndx_end])) {
			break;
		}

		ndx_end++;
	}

	if (ndx_end != 3) {
		// no method specified
		dst->error = HTTP_ERROR_BAD_REQUEST;
		return 1;
	}

	if (strncmp(req, "GET", 3)) {
		dst->error = HTTP_ERROR_BAD_REQUEST;
		return 1;
	}

	dst->method = HTTP_METHOD_GET;

	ndx_start = ++ndx_end;
	while (ndx_end < n) {
		if (isspace(req[ndx_end])) {
			break;
		}

		ndx_end++;
	}

	if (ndx_end - ndx_start == 0) {
		// no path specified
		dst->error = HTTP_ERROR_BAD_REQUEST;
		return 1;
	}

	dst->path     = req + ndx_start;
	dst->path_len = ndx_end - ndx_start;

	return 0;
}

/**
 * Tries to fill the http_parser_t buffer by reading as much as it
 * can from the connection to the internal buffer without blocking.
 *
 * ps.: not being able to read (EWOULDBLOCK or EAGAIN) is not seen
 *      as an error (0 is returned).
 */
int
http_parser_try_fill_buffer(http_parser_t* parser)
{
	(void)parser;

	return 0;
}

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
http_parser_try_get_token(http_parser_t* parser)
{
	(void)parser;
	return 0;
}

/**
 *
 */
int
http_parser_try_parse(http_parser_t* parser)
{
	(void)parser;

	switch (parser->state) {
		case HTTP_PARSER_STATE_REQUEST_LINE_METHOD:
			break;
		case HTTP_PARSER_STATE_REQUEST_LINE_PATH:
			break;
		case HTTP_PARSER_STATE_REQUEST_LINE_PROTOCOL:
			break;
		default:
			break;
	}

	return 0;
}
