# Makefile for Module 3: Scheduling Examples

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS = -lm

all: scheduler_simulator
	@echo "✓ Scheduler simulator compiled successfully"
	@echo "Create a workload file and run:"
	@echo "  ./scheduler_simulator fifo workload.txt"

scheduler_simulator: scheduler_simulator.c
	@echo "Compiling scheduler..."
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	@echo "Cleaning..."
	rm -f scheduler_simulator
	rm -f *.o
	@echo "✓ Clean complete"

test: scheduler_simulator
	@echo "=== Creating test workload ==="
	@echo "0 5 1" > test_workload.txt
	@echo "1 3 2" >> test_workload.txt
	@echo "2 8 1" >> test_workload.txt
	@echo "3 6 3" >> test_workload.txt
	@echo ""
	@echo "=== Testing FIFO ==="
	./scheduler_simulator fifo test_workload.txt
	@echo ""
	@echo "=== Testing SJF ==="
	./scheduler_simulator sjf test_workload.txt
	@echo ""
	@echo "=== Testing STCF ==="
	./scheduler_simulator stcf test_workload.txt
	@echo ""
	@echo "=== Testing Round Robin (q=3) ==="
	./scheduler_simulator rr test_workload.txt 3
	@rm -f test_workload.txt

.PHONY: all clean test

