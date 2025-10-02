# Makefile for Module 2: Process Analysis Examples

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS =

SOURCES = 01_process_states.c \
          02_proc_reader.c

BINARIES = $(SOURCES:.c=)

all: $(BINARIES)
	@echo "✓ All examples compiled successfully"

%: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	@echo "Cleaning..."
	rm -f $(BINARIES)
	rm -f *.o
	@echo "✓ Clean complete"

test: all
	@echo "=== Running Tests ==="
	@echo ""
	@echo "Test 1: Process states"
	./01_process_states 7
	@echo ""
	@echo "Test 2: /proc reader"
	./02_proc_reader info 1
	@echo ""
	@echo "✓ All tests passed"

.PHONY: all clean test

