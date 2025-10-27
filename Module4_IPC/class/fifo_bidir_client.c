#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CLIENT_TO_SERVER "/tmp/client_to_server"
#define SERVER_TO_CLIENT "/tmp/server_to_client"

int main(void) {
  int fd_write, fd_read;
  char message[100];
  char response[100];
  ssize_t bytes_read;
  int request_count = 0;

  printf("===================================\n");
  printf("BIDIRECTIONAL FIFO CLIENT\n");
  printf("===================================\n");
  printf("Connecting to server...\n");

  /* Open FIFOs */
  fd_write = open(CLIENT_TO_SERVER, O_WRONLY);
  if (fd_write == -1) {
    perror("open CLIENT_TO_SERVER");
    printf("\n❌ Error: Make sure the SERVER is running first!\n");
    printf("   Run: ./fifo_bidir_server\n");
    return 1;
  }

  fd_read = open(SERVER_TO_CLIENT, O_RDONLY);
  if (fd_read == -1) {
    perror("open SERVER_TO_CLIENT");
    close(fd_write);
    return 1;
  }

  printf("✓ Connected to server!\n");
  printf("\nSend messages to server (it will process and respond)\n");
  printf("Type 'quit' to exit\n");
  printf("-----------------------------------\n");

  while (1) {
    printf("\n[Client]> ");
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

    /* Send request to server */
    if (write(fd_write, message, strlen(message) + 1) == -1) {
      perror("write");
      break;
    }
    request_count++;
    printf("  📤 Request sent (#%d)\n", request_count);

    /* Wait for response from server */
    printf("  ⏳ Waiting for server response...\n");
    bytes_read = read(fd_read, response, sizeof(response) - 1);
    
    if (bytes_read == -1) {
      perror("read");
      break;
    }

    if (bytes_read == 0) {
      printf("\n[ERROR] Server disconnected.\n");
      break;
    }

    response[bytes_read] = '\0';
    printf("  📨 Server response: %s\n", response);
  }

  close(fd_write);
  close(fd_read);
  
  printf("\n-----------------------------------\n");
  printf("Client exiting.\n");
  printf("Total requests sent: %d\n", request_count);
  return 0;
}

