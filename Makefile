CC ?= gcc
CFLAGS := -std=gnu99

# HSTATIC_PORT defines the port that the server should
# listen to.
#
# Make sure you specify a port that you have privileges to
# (ports <1024 need extra privileges).
HSTATIC_PORT =
ifneq ($(HSTATIC_PORT),)
CFLAGS += -DHSTATIC_PORT=$(HSTATIC_PORT)
endif

# HSTATIC_EPOLL_EVENTS defines the maximum number of epoll
# events.
#
# This setting is only relevant if HSTATIC_NONBLOCKING is
# also set (defaults to false).
HSTATIC_EPOLL_EVENTS =
ifneq ($(HSTATIC_EPOLL_EVENTS),)
CFLAGS += -DHSTATIC_EPOLL_EVENTS=$(HSTATIC_EPOLL_EVENTS)
endif


# Internal varriables.
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
