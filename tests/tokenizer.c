#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/tokenizer.h"

typedef struct test_case {
	char*             description;
	char*             buf;
	char*             expected_buf;
	size_t            expected_offset;
	tokenizer_checker checker;
} test_case_t;

test_case_t test_cases[] = {
	{
	  .description     = "case1",
	  .buf             = "abc def",
	  .expected_buf    = "abc",
	  .expected_offset = 0,
	  .checker         = isspace,
	},
	{
	  .description     = "case2",
	  .buf             = "    def",
	  .expected_buf    = "def",
	  .expected_offset = 4,
	  .checker         = isspace,
	},
	{
	  .description     = "case3",
	  .buf             = "   ",
	  .expected_buf    = "",
	  .expected_offset = 3,
	  .checker         = isspace,
	},
	{
	  .description     = "case4",
	  .buf             = "a\r\nb",
	  .expected_buf    = "a",
	  .expected_offset = 0,
	  .checker         = isspace,
	},
	{
	  .description     = "case5",
	  .buf             = "\r\nbc\r\n",
	  .expected_buf    = "bc",
	  .expected_offset = 2,
	  .checker         = isspace,
	},
};

int
main()
{
	int    number_of_testcases = sizeof(test_cases) / sizeof(test_cases[0]);
	int    ret                 = 0;
	size_t offset;

	for (int i = 0; i < number_of_testcases; i++) {
		test_case_t tc = test_cases[i];

		printf("TEST: %s\n", tc.description);

		ret = tokenizer_get_token(
		  tc.buf, strlen(tc.buf), &offset, tc.checker);
		assert(tc.expected_offset == offset);
		assert(strncmp(tc.expected_buf, tc.buf + offset, ret) == 0);
	}

	return 0;
}
