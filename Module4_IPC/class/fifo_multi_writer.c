#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  const char *fifo_path = "/tmp/multi_writer_fifo";
  int fd;
  char message[100];
  char writer_id[20];
  int msg_count = 0;

  /* Get writer ID from command line or use default */
  if (argc > 1) {
    snprintf(writer_id, sizeof(writer_id), "Writer %s", argv[1]);
  } else {
    snprintf(writer_id, sizeof(writer_id), "Writer %d", getpid());
  }

  printf("===================================\n");
  printf("MULTI-WRITER FIFO: %s\n", writer_id);
  printf("===================================\n");
  printf("PID: %d\n", getpid());
  printf("Connecting to FIFO: %s\n\n", fifo_path);

  /* Open FIFO for writing */
  printf("Opening FIFO for writing...\n");
  fd = open(fifo_path, O_WRONLY);
  if (fd == -1) {
    perror("open");
    printf("\n❌ Error: Make sure the READER is running first!\n");
    printf("   Run: ./fifo_multi_reader\n");
    return 1;
  }

  printf("✓ Connected to reader!\n");
  printf("Enter messages (type 'quit' to exit):\n");
  printf("-----------------------------------\n");

  while (1) {
    printf("[%s]> ", writer_id);
    fflush(stdout);
    
    if (fgets(message, sizeof(message), stdin) == NULL) {
      break;
    }

    /* Remove newline */
    message[strcspn(message, "\n")] = 0;

    /* Check for quit */
    if (strcmp(message, "quit") == 0) {
      break;
    }

    /* Skip empty messages */
    if (strlen(message) == 0) {
      continue;
    }

    /* Prepend writer ID to message */
    char full_message[150];
    snprintf(full_message, sizeof(full_message), "[%s]: %s", writer_id, message);

    /* Write to FIFO */
    if (write(fd, full_message, strlen(full_message) + 1) == -1) {
      perror("write");
      break;
    }
    msg_count++;
    printf("  ✓ Sent (message #%d)\n", msg_count);
  }

  close(fd);
  printf("\n%s exiting.\n", writer_id);
  printf("Total messages sent: %d\n", msg_count);
  return 0;
}

