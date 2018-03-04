CC ?= gcc
CFLAGS := -std=gnu99


# Internal varriables
#
# SRCS contains the list of all `.c` sources
# found under `./src`.
SRCS := $(shell find ./src/*.c)

#
# TESTS contains the list of all `.c` sources
# found under `./tests` that are meant for
# testing.
TESTS := $(shell find ./tests/*.c)



# Builds main binary responsible for running
# the HSTATIC server (`./hstatic.out`).
build: ./cmd/main.c $(SRCS)
	$(CC) $(CFLAGS) -Wall $^ -o ./hstatic.out


# Formats any C-related file using the clang-format
# definition at the root of the project.
#
# Make sure you have clang-format installed before
# executing.
fmt:
	find . -name "*.c" -o -name "*.h" | \
		xargs clang-format -style=file -i


# Removes any binary generated.
clean:
	find . -name "*.out" -type f -delete


# Builds the test binaries under `./tests` and then
# executes them.
test: $(TESTS)
	@for test in $(TESTS); do \
		$(CC) $(CFLAGS) $(SRCS) -g $$test -o $$test.out ; \
		$$test.out ; \
	done


.PHONY: build fmt clean test
