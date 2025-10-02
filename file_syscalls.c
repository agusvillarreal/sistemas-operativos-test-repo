/* file_syscalls.c - Direct system call usage */
#include <errno.h>
#include <fcntl.h> // open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read(), write(), close()

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
  int fd_in, fd_out;
  ssize_t bytes_read, bytes_written;
  char buffer[BUFFER_SIZE];

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // SYSTEM CALL: open() - Opens file
  fd_in = open(argv[1], O_RDONLY);
  if (fd_in == -1) {
    perror("Error opening source file");
    exit(EXIT_FAILURE);
  }

  // SYSTEM CALL: open() with flags
  fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd_out == -1) {
    perror("Error opening destination file");
    close(fd_in);
    exit(EXIT_FAILURE);
  }

  // Copy file using system calls
  while ((bytes_read = read(fd_in, buffer, BUFFER_SIZE)) > 0) {
    bytes_written = write(fd_out, buffer, bytes_read);

    if (bytes_written != bytes_read) {
      perror("Write error");
      close(fd_in);
      close(fd_out);
      exit(EXIT_FAILURE);
    }
  }

  if (bytes_read == -1) {
    perror("Read error");
  }

  // SYSTEM CALL: close()
  close(fd_in);
  close(fd_out);

  printf("File copied successfully\n");
  return EXIT_SUCCESS;
}