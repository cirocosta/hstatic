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
