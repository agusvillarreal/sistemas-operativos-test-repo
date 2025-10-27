#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  const char *fifo_path = "/tmp/my_fifo";
  int fd;
  char buffer[100];
  ssize_t bytes_read;

  printf("=== FIFO READER ===\n");
  printf("Opening FIFO for reading...\n");

  /* Open FIFO for reading */
  fd = open(fifo_path, O_RDONLY);
  if (fd == -1) {
    perror("open");
    printf("Make sure to run './fifo' first to create the FIFO!\n");
    return 1;
  }

  printf("FIFO opened successfully!\n");
  printf("Waiting for messages...\n\n");

  while (1) {
    /* Read from FIFO */
    bytes_read = read(fd, buffer, sizeof(buffer));

    if (bytes_read == -1) {
      perror("read");
      break;
    }

    if (bytes_read == 0) {
      printf("\nWriter closed the FIFO. Exiting.\n");
      break;
    }

    printf("Received: %s\n", buffer);
  }

  close(fd);
  printf("Reader exiting.\n");
  return 0;
}
