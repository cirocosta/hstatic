#include <assert.h>
#include <stdio.h>

#include "../src/http.h"

typedef struct test_case {
	char* description;
	char* request_line;
	size_t request_length;
	http_request_t expected;
	http_request_t actual;
	int expected_response;
} test_case_t;

static int
http_request_equals(http_request_t* a, http_request_t* b)
{
	int ok = a->error == b->error && a->method == b->method &&
	         a->path_len == b->path_len;
	if (!ok) {
		return ok;
	}

	if (a->path_len != 0) {
		ok = !(strncmp(a->path, b->path, a->path_len));
	} else if (b->path_len != 0) {
		ok = !(strncmp(a->path, b->path, b->path_len));
	}

	return ok;
}

test_case_t test_cases[] = {
	{
	  .description = "invalid request if 0 length",
	  .request_line = "weird",
	  .request_length = 0,
	  .expected_response = 1,
	  .expected =
	    {
	      .error = HTTP_ERROR_BAD_REQUEST,
	    },
	},
	{
	  .description = "bad req if method is not get",
	  .request_line = "HEAD /path HTTP/1.1\r\n",
	  .request_length = 21,
	  .expected_response = 1,
	  .expected =
	    {
	      .error = HTTP_ERROR_BAD_REQUEST,
	    },
	},
	{
	  .description = "bad req if no path specified",
	  .request_line = "GET\r\n",
	  .request_length = 5,
	  .expected_response = 1,
	  .expected =
	    {
	      .method = HTTP_METHOD_GET,
	      .error = HTTP_ERROR_BAD_REQUEST,
	    },
	},
	{
	  .description = "parses method and path",
	  .request_line = "GET /path HTTP/1.1\r\n",
	  .request_length = 20,
	  .expected_response = 0,
	  .expected =
	    {
	      .method = HTTP_METHOD_GET,
	      .path = "/path",
	      .path_len = 5,
	    },
	},
};

void
test_http_parse_request()
{
	int number_of_testcases = sizeof(test_cases) / sizeof(test_cases[0]);
	int ret = 0;

	for (int i = 0; i < number_of_testcases; i++) {
		test_case_t tc = test_cases[i];

		printf("TEST: %s\n", tc.description);

		ret = http_parse_request(
		  &tc.actual, tc.request_line, tc.request_length);

		assert(ret == tc.expected_response);
		assert(http_request_equals(&tc.expected, &tc.actual));
	}
}

int
main()
{
	test_http_parse_request();
	return 0;
}
