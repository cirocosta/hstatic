#ifndef __TESTS_UTIL_H
#define __TESTS_UTIL_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/uio.h>

/**
 * Creates a file descriptor via `pipe` using the contents
 * from a buffer such that we can simulate reading from a
 * fd.
 */
static int
create_mocked_fd_from_buff(char* buf, int len)
{
	int pipe_fds[2];
	int pid;
	int err;

	err = pipe(pipe_fds);
	if (err == -1) {
		perror("pipe");
		printf("failed to create pipe\n");
		return -1;
	}

	pid = fork();
	switch (pid) {
		case -1:
			perror("fork");
			printf("failed to fork process");
			return -1;
		case 0:
			for (int written = 0; len > written;) {
				struct iovec iov = {
					.iov_base = buf + written,
					.iov_len  = len - written,
				};

				written +=
				  vmsplice(pipe_fds[1], &iov, 1, SPLICE_F_GIFT);
			}
			exit(0);
		default:
			close(pipe_fds[1]);
			break;
	}

	return pipe_fds[0];
}

#endif
