# Makefile for Module 4 IPC Demonstrations

CC = gcc
CFLAGS = -Wall -Wextra -g

# All targets
ALL_TARGETS = fifo fifo_reader fifo_writer \
              fifo_multi_reader fifo_multi_writer \
              fifo_bidir_server fifo_bidir_client \
              pipes queue queue_sender queue_receiver \
              semaphore simple_semaphore cleanup_ipc

all: $(ALL_TARGETS)

# FIFO demos
fifo: fifo.c
	$(CC) $(CFLAGS) -o fifo fifo.c

fifo_reader: fifo_reader.c
	$(CC) $(CFLAGS) -o fifo_reader fifo_reader.c

fifo_writer: fifo_writer.c
	$(CC) $(CFLAGS) -o fifo_writer fifo_writer.c

# Advanced FIFO demos - Multiple Writers
fifo_multi_reader: fifo_multi_reader.c
	$(CC) $(CFLAGS) -o fifo_multi_reader fifo_multi_reader.c

fifo_multi_writer: fifo_multi_writer.c
	$(CC) $(CFLAGS) -o fifo_multi_writer fifo_multi_writer.c

# Advanced FIFO demos - Bidirectional
fifo_bidir_server: fifo_bidir_server.c
	$(CC) $(CFLAGS) -o fifo_bidir_server fifo_bidir_server.c

fifo_bidir_client: fifo_bidir_client.c
	$(CC) $(CFLAGS) -o fifo_bidir_client fifo_bidir_client.c

# Pipe demos
pipes: pipes.c
	$(CC) $(CFLAGS) -o pipes pipes.c

# Message queue demos
queue: queue.c
	$(CC) $(CFLAGS) -o queue queue.c

queue_sender: queue_sender.c
	$(CC) $(CFLAGS) -o queue_sender queue_sender.c

queue_receiver: queue_receiver.c
	$(CC) $(CFLAGS) -o queue_receiver queue_receiver.c

# Semaphore demos
semaphore: semaphore.c
	$(CC) $(CFLAGS) -o semaphore semaphore.c

simple_semaphore: simple_semaphore.c
	$(CC) $(CFLAGS) -o simple_semaphore simple_semaphore.c

# Cleanup utility
cleanup_ipc: cleanup_ipc.c
	$(CC) $(CFLAGS) -o cleanup_ipc cleanup_ipc.c

# Phony targets
.PHONY: all clean fifo-demos advanced-fifo message-queue semaphores help

# Build specific groups
fifo-demos: fifo fifo_reader fifo_writer
	@echo "Basic FIFO demos compiled!"

advanced-fifo: fifo_multi_reader fifo_multi_writer fifo_bidir_server fifo_bidir_client
	@echo "Advanced FIFO demos compiled!"

message-queue: queue queue_sender queue_receiver
	@echo "Message queue demos compiled!"

semaphores: semaphore simple_semaphore
	@echo "Semaphore demos compiled!"

# Clean up
clean:
	rm -f $(ALL_TARGETS)
	rm -f /tmp/my_fifo /tmp/multi_writer_fifo /tmp/client_to_server /tmp/server_to_client
	@echo "Cleaned up binaries and FIFOs"

# Help
help:
	@echo "Available targets:"
	@echo "  all              - Build all demos"
	@echo "  fifo-demos       - Build basic FIFO demos"
	@echo "  advanced-fifo    - Build advanced FIFO demos"
	@echo "  message-queue    - Build message queue demos"
	@echo "  semaphores       - Build semaphore demos"
	@echo "  clean            - Remove all binaries and FIFOs"
	@echo ""
	@echo "Individual programs:"
	@echo "  Basic FIFOs:     fifo, fifo_reader, fifo_writer"
	@echo "  Advanced FIFOs:  fifo_multi_reader, fifo_multi_writer"
	@echo "                   fifo_bidir_server, fifo_bidir_client"
	@echo "  Message Queues:  queue, queue_sender, queue_receiver"
	@echo "  Semaphores:      semaphore, simple_semaphore"
	@echo "  Utilities:       cleanup_ipc"

