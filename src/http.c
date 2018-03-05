#include "./http.h"

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
