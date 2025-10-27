#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main(void) {
  const char *fifo_path = "/tmp/multi_writer_fifo";
  int fd;
  char buffer[100];
  ssize_t bytes_read;
  int message_count = 0;

  printf("===================================\n");
  printf("MULTI-WRITER FIFO READER\n");
  printf("===================================\n");
  printf("This reader can receive messages from MULTIPLE writers!\n\n");

  /* Create the FIFO if it doesn't exist */
  if (mkfifo(fifo_path, 0666) == -1) {
    /* FIFO already exists, that's OK */
  }

  printf("Opening FIFO for reading: %s\n", fifo_path);
  printf("Waiting for writers to connect...\n\n");

  /* Open FIFO for reading */
  fd = open(fifo_path, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  printf("✓ FIFO opened successfully!\n");
  printf("Ready to receive messages from multiple writers...\n");
  printf("(Press Ctrl+C to exit)\n\n");
  printf("-----------------------------------\n");

  while (1) {
    /* Read from FIFO */
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read == -1) {
      perror("read");
      break;
    }

    if (bytes_read == 0) {
      /* All writers closed - wait for new writers */
      printf("\n[INFO] All writers disconnected. Waiting for new connections...\n");
      close(fd);
      
      /* Reopen FIFO (will block until a writer connects) */
      fd = open(fifo_path, O_RDONLY);
      if (fd == -1) {
        perror("reopen");
        break;
      }
      continue;
    }

    buffer[bytes_read] = '\0';
    message_count++;
    printf("📬 Message #%d: %s\n", message_count, buffer);
  }

  close(fd);
  unlink(fifo_path);
  printf("\nReader exiting.\n");
  printf("Total messages received: %d\n", message_count);
  return 0;
}

