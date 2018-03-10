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
