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
	int               n          = 0;
	size_t            buf_offset = 0;
	size_t            read       = 0;
	char*             token;
	tokenizer_checker tokenizer_checker = isspace;

	while (1) {
		read += (n + buf_offset);

		n = tokenizer_get_token(parser->buf + read,
		                        parser->buf_len - read,
		                        &buf_offset,
		                        tokenizer_checker);
		if (n == -1) {
			printf(
			  "unexpected error while retrieving token from buf\n");
			return -1;
		}

		if (n == 0) {
			return 0;
		}

		token = parser->buf + read + buf_offset;
		printf("token=%.*s\n", n, token);
		printf("n=%d\n", n);

		switch (parser->state) {
			case HTTP_PARSER_STATE_NEW:
				if (n == 3 && strncmp(token, "GET", 3) != 0) {
					printf(
					  "didn't detect a valid method\n");
					return -1;
				}

				parser->state =
				  HTTP_PARSER_STATE_REQUEST_LINE_METHOD;
				parser->req.method = HTTP_METHOD_GET;

				printf("[info] METHOD\n");
				break;

			case HTTP_PARSER_STATE_REQUEST_LINE_METHOD:
				if (token[0] != '/') {
					printf("didn't detect a proper path\n");
					return -1;
				}

				parser->req.path     = token;
				parser->req.path_len = n;
				parser->state =
				  HTTP_PARSER_STATE_REQUEST_LINE_PATH;

				printf("[info] PATH\n");

				break;

			case HTTP_PARSER_STATE_REQUEST_LINE_PATH:
				if (n != 8 ||
				    strncmp(token, "HTTP/1.1", 8) != 0) {
					printf("didn't detect a proper "
					       "protocol - %s\n",
					       token);
					return -1;
				}

				printf("[info] PROTOCOL\n");

				parser->state =
				  HTTP_PARSER_STATE_REQUEST_LINE_PROTOCOL;
				tokenizer_checker = tokenizer_checker_iscolon;

				break;

			case HTTP_PARSER_STATE_REQUEST_LINE_PROTOCOL:
			case HTTP_PARSER_STATE_HEADER_VALUE:
				printf("[info] HEADER KEY\n");

				if (n < 4) {
					break;
				}

				if (token[n - 4] == '\r' &&
				    token[n - 3] == '\n' &&
				    token[n - 2] == '\r' &&
				    token[n - 1] == '\n') {
					parser->state = HTTP_PARSER_STATE_DONE;
					break;
				}

				parser->state = HTTP_PARSER_STATE_HEADER_KEY;
				tokenizer_checker = tokenizer_checker_iscrlf;

				break;

			case HTTP_PARSER_STATE_HEADER_KEY:
				// we just got the key of the header (`Host:`)

				printf("[info] HEADER VALUE\n");

				parser->state = HTTP_PARSER_STATE_HEADER_VALUE;
				tokenizer_checker = tokenizer_checker_iscolon;

				// if the last four characters are `\r\n\r\n`,
				// then
				// we finish the header section.
				if (n < 4) {
					break;
				}

				if (token[n - 4] == '\r' &&
				    token[n - 3] == '\n' &&
				    token[n - 2] == '\r' &&
				    token[n - 1] == '\n') {
					parser->state = HTTP_PARSER_STATE_DONE;
					break;
				}

				break;

			case HTTP_PARSER_STATE_DONE:
				return 0;

			default:
				printf("unexpected state");
				return -1;
		}
	}

	return 0;
}
