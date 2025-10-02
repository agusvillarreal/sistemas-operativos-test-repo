# Makefile for Module 4: IPC Examples

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS = -lrt -lpthread

SOURCES = 01_pipes.c 02_shared_memory.c
BINARIES = $(SOURCES:.c=)

all: $(BINARIES)
	@echo "✓ All IPC examples compiled"

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(BINARIES) *.o

test: all
	@echo "=== Testing Pipes ==="
	./01_pipes
	@echo ""
	@echo "=== Testing Shared Memory ==="
	@echo "Run in two terminals:"
	@echo "  Terminal 1: ./02_shared_memory writer"
	@echo "  Terminal 2: ./02_shared_memory reader"

.PHONY: all clean test

