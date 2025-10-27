#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int pipefd[2];
  pid_t pid;
  char write_msg[] = "Message from parent process";
  char read_msg[100];

  /* Create the pipe before forking */
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    /* Child process - will read from pipe */
    close(pipefd[1]); /* Close write end */

    ssize_t bytes_read = read(pipefd[0], read_msg, sizeof(read_msg) - 1);
    if (bytes_read > 0) {
      read_msg[bytes_read] = '\0';
      printf("Child received: %s\n", read_msg);
    }

    close(pipefd[0]);
    exit(EXIT_SUCCESS);
  } else {
    /* Parent process - will write to pipe */
    close(pipefd[0]); /* Close read end */

    write(pipefd[1], write_msg, strlen(write_msg));

    close(pipefd[1]);
    wait(NULL); /* Wait for child to complete */
  }

  return 0;
}