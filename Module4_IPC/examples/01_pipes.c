/*
 * 01_pipes.c - Pipe examples
 * Demonstrates anonymous pipes for parent-child communication
 *
 * Compile: gcc -o pipes 01_pipes.c
 * Run: ./pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Example 1: Basic pipe */
void example_basic_pipe() {
  printf("\n=== Example 1: Basic Pipe ===\n");

  int pipefd[2];
  pipe(pipefd);

  pid_t pid = fork();

  if (pid == 0) {
    // Child: reader
    close(pipefd[1]); // Close unused write end

    char buffer[100];
    int n = read(pipefd[0], buffer, sizeof(buffer));
    buffer[n] = '\0';

    printf("[Child] Received: %s\n", buffer);

    close(pipefd[0]);
    exit(0);
  } else {
    // Parent: writer
    close(pipefd[0]); // Close unused read end

    const char *msg = "Hello from parent!";
    write(pipefd[1], msg, strlen(msg));

    close(pipefd[1]);
    wait(NULL);
  }
}

/* Example 2: Bidirectional communication (2 pipes) */
void example_bidirectional() {
  printf("\n=== Example 2: Bidirectional Communication ===\n");

  int pipe1[2], pipe2[2];
  pipe(pipe1); // Parent → Child
  pipe(pipe2); // Child → Parent

  pid_t pid = fork();

  if (pid == 0) {
    // Child
    close(pipe1[1]); // Close unused write end of pipe1
    close(pipe2[0]); // Close unused read end of pipe2

    // Read from parent
    char buffer[100];
    read(pipe1[0], buffer, sizeof(buffer));
    printf("[Child] Received from parent: %s\n", buffer);

    // Send to parent
    write(pipe2[1], "Reply from child", 16);

    close(pipe1[0]);
    close(pipe2[1]);
    exit(0);
  } else {
    // Parent
    close(pipe1[0]); // Close unused read end of pipe1
    close(pipe2[1]); // Close unused write end of pipe2

    // Send to child
    write(pipe1[1], "Message from parent", 19);

    // Read from child
    char buffer[100];
    read(pipe2[0], buffer, sizeof(buffer));
    printf("[Parent] Received from child: %s\n", buffer);

    close(pipe1[1]);
    close(pipe2[0]);
    wait(NULL);
  }
}

/* Example 3: Pipeline (like shell pipes) */
void example_pipeline() {
  printf("\n=== Example 3: Pipeline (P1 | P2 | P3) ===\n");

  int pipe1[2], pipe2[2];
  pipe(pipe1);
  pipe(pipe2);

  // Process 1: Generator
  if (fork() == 0) {
    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);

    dup2(pipe1[1], STDOUT_FILENO);
    close(pipe1[1]);

    printf("100\n200\n300\n");
    exit(0);
  }

  // Process 2: Filter
  if (fork() == 0) {
    close(pipe1[1]);
    close(pipe2[0]);

    dup2(pipe1[0], STDIN_FILENO);
    dup2(pipe2[1], STDOUT_FILENO);
    close(pipe1[0]);
    close(pipe2[1]);

    int num;
    while (scanf("%d", &num) == 1) {
      printf("%d\n", num * 2); // Double each number
    }
    exit(0);
  }

  // Process 3: Consumer
  close(pipe1[0]);
  close(pipe1[1]);
  close(pipe2[1]);

  char line[100];
  FILE *fp = fdopen(pipe2[0], "r");
  printf("[Process 3] Results:\n");
  while (fgets(line, sizeof(line), fp)) {
    printf("  %s", line);
  }
  fclose(fp);

  wait(NULL);
  wait(NULL);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int ex = atoi(argv[1]);
    switch (ex) {
    case 1:
      example_basic_pipe();
      break;
    case 2:
      example_bidirectional();
      break;
    case 3:
      example_pipeline();
      break;
    default:
      printf("Example 1-3\n");
    }
  } else {
    example_basic_pipe();
    example_bidirectional();
    example_pipeline();
  }
  return 0;
}
