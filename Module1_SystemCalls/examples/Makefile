# Makefile for Module 1: System Calls Examples
# Usage:
#   make          - Build all examples
#   make clean    - Remove all binaries
#   make test     - Run basic tests
#   make trace    - Run with strace

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS = 

# Source files
SOURCES = 01_basic_syscalls.c \
          02_process_creation.c \
          03_signal_handling.c

# Binary names (remove .c extension)
BINARIES = $(SOURCES:.c=)

# Default target
all: $(BINARIES)
	@echo "✓ All examples compiled successfully"
	@echo "Run './01_basic_syscalls' to start"

# Pattern rule to compile each .c file
%: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Individual targets
01_basic_syscalls: 01_basic_syscalls.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

02_process_creation: 02_process_creation.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

03_signal_handling: 03_signal_handling.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Clean build artifacts
clean:
	@echo "Cleaning..."
	rm -f $(BINARIES)
	rm -f *.o
	rm -f test_syscall.txt
	@echo "✓ Clean complete"

# Run basic tests
test: all
	@echo "=== Running Tests ==="
	@echo ""
	@echo "Test 1: Basic syscalls"
	./01_basic_syscalls
	@echo ""
	@echo "Test 2: Process creation (example 1)"
	./02_process_creation 1
	@echo ""
	@echo "Test 3: Signal handling (example 4 - alarm)"
	./03_signal_handling 4
	@echo ""
	@echo "✓ All tests passed"

# Run with strace
trace: all
	@echo "=== Tracing System Calls ==="
	@echo ""
	@echo "Tracing 01_basic_syscalls:"
	strace -e trace=open,read,write,close,stat,unlink ./01_basic_syscalls
	@echo ""

# Run with valgrind
valgrind: all
	@echo "=== Memory Check ==="
	valgrind --leak-check=full --show-leak-kinds=all ./02_process_creation 5

# Debug with gdb
debug: all
	@echo "Starting GDB on 02_process_creation..."
	gdb -ex "break main" -ex "run" ./02_process_creation

# Help target
help:
	@echo "Available targets:"
	@echo "  make          - Build all examples"
	@echo "  make clean    - Remove binaries"
	@echo "  make test     - Run basic tests"
	@echo "  make trace    - Run with strace"
	@echo "  make valgrind - Check memory leaks"
	@echo "  make debug    - Start GDB debugger"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Individual programs:"
	@echo "  ./01_basic_syscalls"
	@echo "  ./02_process_creation [1-5]"
	@echo "  ./03_signal_handling [1-7]"

.PHONY: all clean test trace valgrind debug help

