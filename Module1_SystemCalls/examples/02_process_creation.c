/*
 * 02_process_creation.c
 *
 * Demonstrates process creation with fork(), exec(), and wait()
 * Shows process hierarchy and parent-child relationships
 *
 * Compile: gcc -o process_creation 02_process_creation.c
 * Run: ./process_creation
 * Trace: strace -f ./process_creation  (note: -f follows forks)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CHILDREN 3

/* Helper function to print process information */
void print_process_info(const char *label) {
  printf("[%s] PID=%d, PPID=%d, PGID=%d\n", label, getpid(), getppid(),
         getpgrp());
}

/* Example 1: Basic fork() */
void example_basic_fork() {
  printf("\n=== Example 1: Basic fork() ===\n");

  int shared_var = 100;

  printf("Before fork():\n");
  print_process_info("Parent");

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    /* CHILD PROCESS */
    shared_var = 200; // Child modifies variable
    printf("\nAfter fork() - CHILD:\n");
    print_process_info("Child");
    printf("shared_var in child = %d\n", shared_var);
    printf("Child sleeping for 2 seconds...\n");
    sleep(2);
    printf("Child exiting\n");
    exit(42); // Exit with status 42
  } else {
    /* PARENT PROCESS */
    printf("\nAfter fork() - PARENT:\n");
    print_process_info("Parent");
    printf("shared_var in parent = %d\n", shared_var);
    printf("Child PID = %d\n", pid);

    // Wait for child
    int status;
    pid_t child_pid = wait(&status);

    printf("\nChild %d terminated\n", child_pid);
    if (WIFEXITED(status)) {
      printf("Child exited normally with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("Child killed by signal %d\n", WTERMSIG(status));
    }
  }
}

/* Example 2: Multiple children */
void example_multiple_children() {
  printf("\n\n=== Example 2: Creating Multiple Children ===\n");

  pid_t children[MAX_CHILDREN];

  for (int i = 0; i < MAX_CHILDREN; i++) {
    pid_t pid = fork();

    if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      /* CHILD PROCESS */
      printf("[Child %d] PID=%d started\n", i, getpid());

      // Each child sleeps for different duration
      sleep(i + 1);

      printf("[Child %d] PID=%d exiting with status %d\n", i, getpid(), i * 10);
      exit(i * 10);
    } else {
      /* PARENT PROCESS */
      children[i] = pid;
      printf("[Parent] Created child %d with PID=%d\n", i, pid);
    }
  }

  /* Parent waits for all children */
  printf("\n[Parent] Waiting for all children to finish...\n");

  for (int i = 0; i < MAX_CHILDREN; i++) {
    int status;
    pid_t pid = wait(&status);

    if (WIFEXITED(status)) {
      printf("[Parent] Child PID=%d exited with status %d\n", pid,
             WEXITSTATUS(status));
    }
  }

  printf("[Parent] All children completed\n");
}

/* Example 3: fork() with exec() */
void example_fork_exec() {
  printf("\n\n=== Example 3: fork() with exec() ===\n");

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    /* CHILD PROCESS - will replace itself */
    printf("[Child] About to exec 'ls -l'\n");
    printf("[Child] My PID=%d will remain the same after exec\n", getpid());

    // Replace this process with 'ls'
    char *args[] = {"ls", "-l", "/tmp", NULL};
    char *env[] = {NULL};

    execve("/bin/ls", args, env);

    // Only reached if exec fails
    perror("execve failed");
    exit(EXIT_FAILURE);
  } else {
    /* PARENT PROCESS */
    printf("[Parent] Waiting for child (PID=%d) to complete exec...\n", pid);

    int status;
    wait(&status);

    printf("[Parent] Child completed\n");
  }
}

/* Example 4: Process tree visualization */
void example_process_tree() {
  printf("\n\n=== Example 4: Process Tree ===\n");
  printf("Creating a tree of processes\n");
  printf("Use 'pstree -p %d' in another terminal to see the tree\n\n",
         getpid());

  pid_t pid1 = fork();

  if (pid1 == 0) {
    /* First child */
    printf("[Child 1] PID=%d\n", getpid());

    // First child creates its own child (grandchild of original)
    pid_t pid_grandchild = fork();

    if (pid_grandchild == 0) {
      /* Grandchild */
      printf("  [Grandchild] PID=%d, PPID=%d\n", getpid(), getppid());
      sleep(3);
      exit(0);
    }

    wait(NULL); // Child 1 waits for grandchild
    exit(0);
  }

  pid_t pid2 = fork();

  if (pid2 == 0) {
    /* Second child */
    printf("[Child 2] PID=%d\n", getpid());
    sleep(3);
    exit(0);
  }

  /* Parent waits for both children */
  printf("[Parent] PID=%d, waiting for children...\n", getpid());
  printf("[Parent] Sleeping for 3 seconds - check process tree now!\n");
  sleep(3);

  wait(NULL);
  wait(NULL);

  printf("[Parent] All done\n");
}

/* Example 5: Demonstrating copy-on-write */
void example_copy_on_write() {
  printf("\n\n=== Example 5: Copy-on-Write Demonstration ===\n");

// Allocate large array (1MB)
#define ARRAY_SIZE (1024 * 1024)
  int *large_array = malloc(ARRAY_SIZE * sizeof(int));

  if (!large_array) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  // Initialize array
  for (int i = 0; i < ARRAY_SIZE; i++) {
    large_array[i] = i;
  }

  printf("Allocated 1MB array at %p\n", (void *)large_array);
  printf("Forking... (Copy-on-Write will prevent full copy)\n");

  pid_t pid = fork();

  if (pid == 0) {
    /* CHILD PROCESS */
    printf("[Child] Array pointer: %p\n", (void *)large_array);
    printf("[Child] Reading array[0] = %d (no copy yet)\n", large_array[0]);
    sleep(1);

    printf("[Child] Modifying array[0] (triggers copy-on-write)\n");
    large_array[0] = 99999;

    printf("[Child] New value: array[0] = %d\n", large_array[0]);

    free(large_array);
    exit(0);
  } else {
    /* PARENT PROCESS */
    printf("[Parent] Array pointer: %p\n", (void *)large_array);

    sleep(2); // Let child modify first

    printf("[Parent] array[0] = %d (unchanged in parent)\n", large_array[0]);

    wait(NULL);
    free(large_array);
  }
}

int main(int argc, char *argv[]) {
  printf("=== Process Creation Demonstration ===\n");
  printf("Original process PID: %d\n", getpid());

  if (argc > 1) {
    int example = atoi(argv[1]);
    switch (example) {
    case 1:
      example_basic_fork();
      break;
    case 2:
      example_multiple_children();
      break;
    case 3:
      example_fork_exec();
      break;
    case 4:
      example_process_tree();
      break;
    case 5:
      example_copy_on_write();
      break;
    default:
      printf("Usage: %s [1-5]\n", argv[0]);
      printf("  1: Basic fork\n");
      printf("  2: Multiple children\n");
      printf("  3: fork + exec\n");
      printf("  4: Process tree\n");
      printf("  5: Copy-on-write\n");
    }
  } else {
    // Run all examples
    example_basic_fork();
    example_multiple_children();
    example_fork_exec();
    example_process_tree();
    example_copy_on_write();
  }

  printf("\n=== All Examples Complete ===\n");
  return EXIT_SUCCESS;
}

/*
 * TESTING COMMANDS:
 *
 * 1. Compile and run:
 *    gcc -o process_creation 02_process_creation.c
 *    ./process_creation
 *
 * 2. Run specific example:
 *    ./process_creation 1
 *
 * 3. Trace with strace (follow forks):
 *    strace -f -e trace=fork,execve,wait4,exit_group ./process_creation 1
 *
 * 4. Monitor with ps (in another terminal):
 *    watch -n 0.5 "ps -f -p $(pgrep -f process_creation)"
 *
 * 5. View process tree:
 *    pstree -p <parent_pid>
 *
 * EXERCISES:
 *
 * 1. Modify example_multiple_children() to use waitpid() instead of wait()
 *    to wait for children in specific order
 *
 * 2. Create a process chain: P1 → P2 → P3 → P4 (each creates next)
 *
 * 3. Implement a process pool with N worker processes
 *
 * 4. Handle SIGCHLD signal to reap children asynchronously
 */
