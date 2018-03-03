SRCS := $(shell find ./src/*.c)
TESTS := $(shell find ./tests/*.c)

build: ./cmd/main.c $(SRCS)
	gcc -Wall $^ -o ./server.out

fmt:
	find . -name "*.c" -o -name "*.h" | \
		xargs clang-format -style=file -i

clean:
	find . -name "*.out" -type f -delete

test: $(TESTS)
	@for test in $(TESTS); do \
		gcc $(SRCS) -g $$test -o $$test.out ; \
		$$test.out ; \
	done

.PHONY: fmt clean test
