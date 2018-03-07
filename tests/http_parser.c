#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Creates a file descriptor via `pipe` using the contents
 * from a buffer such that we can simulate reading from a
 * fd.
 */
int
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
			for (int written = 0; len > written;)
				written += write(
				  pipe_fds[1], buf + written, len - written);
			exit(0);
		default:
			close(pipe_fds[1]);
			break;
	}

	return pipe_fds[0];
}

int
main()
{
	int  fd = create_mocked_fd_from_buff("abc", 3);
	int  n;
	char buf[3];

	while (1) {
		n = read(fd, buf, 3);
		if (!n) {
			break;
		}
	}

	printf("READ:%s\n", buf);

	return 0;
}
