#ifndef __FD_H
#define __FD_H

#include <fcntl.h>
#include <stdio.h>

/**
 * Turns a socket into non-blocking by changing
 * its flags using `fcntl`.
 */
int
fd_make_nonblocking(int socket);

#endif
