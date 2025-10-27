#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>

#define CLIENT_TO_SERVER "/tmp/client_to_server"
#define SERVER_TO_CLIENT "/tmp/server_to_client"

int main(void) {
  int fd_read, fd_write;
  char buffer[100];
  char response[100];
  ssize_t bytes_read;
  int request_count = 0;

  printf("===================================\n");
  printf("BIDIRECTIONAL FIFO SERVER\n");
  printf("===================================\n");
  printf("Creating FIFOs...\n");

  /* Create both FIFOs */
  if (mkfifo(CLIENT_TO_SERVER, 0666) == -1) {
    /* FIFO already exists, remove and recreate */
    unlink(CLIENT_TO_SERVER);
    mkfifo(CLIENT_TO_SERVER, 0666);
  }
  if (mkfifo(SERVER_TO_CLIENT, 0666) == -1) {
    unlink(SERVER_TO_CLIENT);
    mkfifo(SERVER_TO_CLIENT, 0666);
  }

  printf("  ✓ Created: %s\n", CLIENT_TO_SERVER);
  printf("  ✓ Created: %s\n\n", SERVER_TO_CLIENT);

  printf("Waiting for client to connect...\n");

  /* Open FIFOs */
  fd_read = open(CLIENT_TO_SERVER, O_RDONLY);
  if (fd_read == -1) {
    perror("open CLIENT_TO_SERVER");
    return 1;
  }

  fd_write = open(SERVER_TO_CLIENT, O_WRONLY);
  if (fd_write == -1) {
    perror("open SERVER_TO_CLIENT");
    close(fd_read);
    return 1;
  }

  printf("✓ Client connected!\n");
  printf("Server ready to process requests...\n");
  printf("(Press Ctrl+C to exit)\n\n");
  printf("-----------------------------------\n");

  while (1) {
    /* Read request from client */
    bytes_read = read(fd_read, buffer, sizeof(buffer) - 1);

    if (bytes_read == -1) {
      perror("read");
      break;
    }

    if (bytes_read == 0) {
      printf("\n[INFO] Client disconnected.\n");
      break;
    }

    buffer[bytes_read] = '\0';
    request_count++;
    
    printf("📨 Request #%d from client: %s\n", request_count, buffer);

    /* Process the request (convert to uppercase as example) */
    snprintf(response, sizeof(response), "PROCESSED: ");
    size_t offset = strlen(response);
    for (size_t i = 0; buffer[i] && offset < sizeof(response) - 1; i++) {
      response[offset++] = toupper(buffer[i]);
    }
    response[offset] = '\0';

    /* Send response back to client */
    if (write(fd_write, response, strlen(response) + 1) == -1) {
      perror("write");
      break;
    }
    printf("📤 Response sent: %s\n\n", response);
  }

  close(fd_read);
  close(fd_write);
  unlink(CLIENT_TO_SERVER);
  unlink(SERVER_TO_CLIENT);
  
  printf("-----------------------------------\n");
  printf("Server shutting down.\n");
  printf("Total requests processed: %d\n", request_count);
  return 0;
}

