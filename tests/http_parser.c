#include <assert.h>

#include "../src/http_parser.h"
#include "./util.h"

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
test_parses_request()
{
	int            fd;
	int            err;
	http_parser_t* parser;
	char           buf[] = "GET / HTTP/1.1\r\n";

	fd = create_mocked_fd_from_buff(buf, sizeof(buf) - 1);
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
	assert(parser->req.method == HTTP_METHOD_GET);
	assert(strncmp(parser->req.path, "/", 1) == 0);
	assert(parser->state == HTTP_PARSER_STATE_DONE);
}

int
main()
{
	test_fills_buffer_completely();
	test_parses_request();

	return 0;
}
