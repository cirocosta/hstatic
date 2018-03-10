#ifndef __SRC_TOKENIZER_H
#define __SRC_TOKENIZER_H

#include <ctype.h>
#include <stdlib.h>

/**
 * Verifies whether the character can be part
 * of a token or not.
 */
typedef int (*tokenizer_checker)(int);

/**
 * Looks up for a token in `buf` of size `len`.
 *
 * It returns the start of the token in `buf` in the offset
 * pointer variable.
 *
 * RETURN VALUE
 *
 *      1       error
 *      >= 0    size of the token
 *
 * EXAMPLES
 *
 *      'abc'   --> offset=buf+0; ret=3
 *      'a c'   --> offset=buf+0; ret=1
 *      '  c'   --> offset=buf+2; ret=1
 *      '   '   --> offset=buf+0; ret=0
 */
int
tokenizer_get_token(char*             buf,
                    size_t            buf_len,
                    size_t*           offset,
                    tokenizer_checker checker);

static int
tokenizer_checker_iscolon(int c)
{
	return c == ':';
}

static int
tokenizer_checker_iscrlf(int c)
{
	return c == '\r' || c == '\n';
}

#endif
