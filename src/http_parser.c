#include "./http_parser.h"

http_parser_t*
http_parser_create(connection_t* conn)
{
	http_parser_t* parser;

	if (conn == NULL) {
		printf("a connection must be supplied\n");
		return NULL;
	}

	parser = malloc(sizeof(*parser));
	if (parser == NULL) {
		perror("malloc");
		printf("failed to create parser struct\n");
		return parser;
	}

	parser->conn  = conn;
	parser->state = HTTP_PARSER_STATE_NEW;

	return parser;
}

int
http_parser_try_fill_buffer(http_parser_t* parser)
{
	int n = 0;

	while (1) {
		n = read(parser->conn->fd,
		         parser->buf,
		         HSTATIC_HTTP_PARSER_BUFFER_SIZE);
		if (n == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return 0;
			}

			perror("read");
			printf("failed to read from connection\n");
			return -1;
		}

		if (n == 0) {
			return 0;
		}

		parser->buf_len = n;
	}

	return 0;
}

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
