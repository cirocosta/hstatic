#include "./tokenizer.h"

int
tokenizer_get_token(char*             buf,
                    size_t            buf_len,
                    size_t*           offset,
                    tokenizer_checker check)
{
	size_t ndx_start = 0;
	size_t ndx_end   = 0;

	// remove all the spaces
	while (ndx_start < buf_len && check(buf[ndx_start])) {
		ndx_start++;
		ndx_end++;
	}

	// count the non-spaces
	while (ndx_end < buf_len) {
		if (check(buf[ndx_end])) {
			break;
		}

		ndx_end++;
	}

	*offset = ndx_start;
	return ndx_end - ndx_start;
}
