#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  const char *fifo_path = "/tmp/my_fifo";
  int fd;
  char message[100];

  printf("=== FIFO WRITER ===\n");
  printf("Opening FIFO for writing...\n");

  /* Open FIFO for writing */
  fd = open(fifo_path, O_WRONLY);
  if (fd == -1) {
    perror("open");
    printf("Make sure to run './fifo' first to create the FIFO!\n");
    return 1;
  }

  printf("FIFO opened successfully!\n");
  printf("Enter messages (type 'quit' to exit):\n");

  while (1) {
    printf("> ");
    if (fgets(message, sizeof(message), stdin) == NULL) {
      break;
    }

    /* Remove newline */
    message[strcspn(message, "\n")] = 0;

    /* Check for quit */
    if (strcmp(message, "quit") == 0) {
      break;
    }

    /* Write to FIFO */
    if (write(fd, message, strlen(message) + 1) == -1) {
      perror("write");
      break;
    }
    printf("Sent: %s\n", message);
  }

  close(fd);
  printf("Writer exiting.\n");
  return 0;
}
