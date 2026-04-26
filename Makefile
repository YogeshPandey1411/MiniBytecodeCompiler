CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Iinclude
SRCS    = src/main.c src/lexer.c src/parser.c src/codegen.c src/vm.c
OBJS    = $(SRCS:.c=.o)
TARGET  = compiler

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET)

# Run all test files in tests/
test: all
	@echo "\n=== Running tests ==="; \
	passed=0; failed=0; \
	for f in tests/*.src; do \
		echo "\n--- $$f ---"; \
		./$(TARGET) "$$f" && passed=$$((passed+1)) || failed=$$((failed+1)); \
	done; \
	echo "\n=== Results: $$passed passed, $$failed failed ===\n"

clean:
	rm -f $(OBJS) $(TARGET)
