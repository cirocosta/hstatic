SRCS := $(shell find ./src/*.c)

build: $(SRCS)
	gcc $^ -o ./server.out

fmt:
	find . -name "*.c" -o -name "*.h" | \
		xargs clang-format -style=file -i

clean:
	find . -name "*.out" -type f -delete

test:
	true


.PHONY: fmt clean test
