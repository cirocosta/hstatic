#include <assert.h>

#include "../src/http_parser.h"
#include "./util.h"

typedef struct test_case {
	char*               description;
	char*               buf;
	http_request_t      expected_request;
	http_parser_state_e expected_state;
} test_case_t;

test_case_t test_cases[] = {
	{
	  .description    = "request line",
	  .buf            = "GET / HTTP/1.1\r\n",
	  .expected_state = HTTP_PARSER_STATE_REQUEST_LINE_PROTOCOL,
	},
	{
	  .description = "single header",
	  .buf         = "GET / HTTP/1.1\r\n"
	         "Host: test.com\r\n"
	         "\r\n",
	  .expected_state = HTTP_PARSER_STATE_DONE,
	},
	{
	  .description = "multiple headers",
	  .buf         = "GET / HTTP/1.1\r\n"
	         "Host: test.com\r\n"
	         "Blabla: hehe\r\n"
	         "\r\n",
	  .expected_state = HTTP_PARSER_STATE_DONE,
	},
};

void
test_fills_buffer_completely()
{
	char           buf[] = "abc";
	http_parser_t* parser;
	int            fd;
	int            err;

	fd = create_mocked_fd_from_buff(buf, sizeof(buf) - 1);
	assert(fd != -1);

	parser = http_parser_create(&((connection_t){
	  .fd = fd,
	}));
	assert(parser != NULL);

	err = http_parser_try_fill_buffer(parser);
	assert(err == 0);

	assert(parser->buf_len == 3);
	assert(strncmp(parser->buf, buf, 3) == 0);
}

void
test_parses_request(test_case_t* tc)
{
	int            fd;
	int            err;
	http_parser_t* parser;

	fd = create_mocked_fd_from_buff(tc->buf, strlen(tc->buf));
	assert(fd != -1);

	parser = http_parser_create(&((connection_t){
	  .fd = fd,
	}));
	assert(parser != NULL);
	assert(parser->state == HTTP_PARSER_STATE_NEW);

	err = http_parser_try_fill_buffer(parser);
	assert(err == 0);

	err = http_parser_try_parse(parser);
	assert(err == 0);
	assert(parser->state == tc->expected_state);
}

int
main()
{
	test_fills_buffer_completely();

	int number_of_testcases = sizeof(test_cases) / sizeof(test_cases[0]);

	for (int i = 0; i < number_of_testcases; i++) {
		printf("TEST: %s\n", test_cases[i].description);
		test_parses_request(&test_cases[i]);
	}

	return 0;
}
