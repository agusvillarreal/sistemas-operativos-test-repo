/*
 * 01_basic_syscalls.c
 *
 * Demonstrates basic system call usage in C
 * Shows: open(), read(), write(), close(), stat()
 *
 * Compile: gcc -o basic_syscalls 01_basic_syscalls.c
 * Run: ./basic_syscalls
 * Trace: strace ./basic_syscalls
 */

#include <errno.h>
#include <fcntl.h> // open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // stat()
#include <unistd.h>   // read(), write(), close()

#define TEST_FILE "test_syscall.txt"
#define BUFFER_SIZE 1024

/* Print file information using stat() */
void print_file_info(const char *filename) {
  struct stat sb;

  printf("\n=== File Information ===\n");

  if (stat(filename, &sb) == -1) {
    perror("stat");
    return;
  }

  printf("File: %s\n", filename);
  printf("Size: %ld bytes\n", sb.st_size);
  printf("Inode: %lu\n", sb.st_ino);
  printf("Permissions: %o\n", sb.st_mode & 0777);
  printf("Links: %lu\n", sb.st_nlink);
  printf("UID: %d, GID: %d\n", sb.st_uid, sb.st_gid);
}

int main() {
  int fd;
  ssize_t bytes_written, bytes_read;
  char write_buffer[] = "Hello from system calls!\nThis is line 2.\n";
  char read_buffer[BUFFER_SIZE];

  printf("=== System Call Demonstration ===\n");
  printf("PID: %d\n", getpid());

  /* ============================================
   * WRITE OPERATION
   * ============================================ */
  printf("\n[1] Creating and writing to file...\n");

  // Open file for writing (create if doesn't exist)
  // O_WRONLY: write only
  // O_CREAT: create if doesn't exist
  // O_TRUNC: truncate to zero length if exists
  // 0644: permissions (rw-r--r--)
  fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    perror("open (write)");
    exit(EXIT_FAILURE);
  }
  printf("File opened for writing, fd=%d\n", fd);

  // Write data
  bytes_written = write(fd, write_buffer, strlen(write_buffer));
  if (bytes_written == -1) {
    perror("write");
    close(fd);
    exit(EXIT_FAILURE);
  }
  printf("Wrote %ld bytes\n", bytes_written);

  // Close file
  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }
  printf("File closed\n");

  /* ============================================
   * READ OPERATION
   * ============================================ */
  printf("\n[2] Reading from file...\n");

  // Open file for reading
  fd = open(TEST_FILE, O_RDONLY);
  if (fd == -1) {
    perror("open (read)");
    exit(EXIT_FAILURE);
  }
  printf("File opened for reading, fd=%d\n", fd);

  // Read data
  memset(read_buffer, 0, BUFFER_SIZE);
  bytes_read = read(fd, read_buffer, BUFFER_SIZE - 1);
  if (bytes_read == -1) {
    perror("read");
    close(fd);
    exit(EXIT_FAILURE);
  }
  printf("Read %ld bytes\n", bytes_read);
  printf("Content:\n%s", read_buffer);

  close(fd);

  /* ============================================
   * FILE INFORMATION
   * ============================================ */
  print_file_info(TEST_FILE);

  /* ============================================
   * APPEND OPERATION
   * ============================================ */
  printf("\n[3] Appending to file...\n");

  fd = open(TEST_FILE, O_WRONLY | O_APPEND);
  if (fd == -1) {
    perror("open (append)");
    exit(EXIT_FAILURE);
  }

  char append_data[] = "This line was appended!\n";
  bytes_written = write(fd, append_data, strlen(append_data));
  printf("Appended %ld bytes\n", bytes_written);

  close(fd);

  /* ============================================
   * VERIFY APPEND
   * ============================================ */
  printf("\n[4] Reading entire file after append...\n");

  fd = open(TEST_FILE, O_RDONLY);
  if (fd == -1) {
    perror("open (verify)");
    exit(EXIT_FAILURE);
  }

  memset(read_buffer, 0, BUFFER_SIZE);
  bytes_read = read(fd, read_buffer, BUFFER_SIZE - 1);
  printf("Content after append:\n%s", read_buffer);

  close(fd);

  /* ============================================
   * CLEANUP
   * ============================================ */
  printf("\n[5] Deleting test file...\n");
  if (unlink(TEST_FILE) == -1) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }
  printf("File deleted successfully\n");

  printf("\n=== Program Complete ===\n");
  return EXIT_SUCCESS;
}

/*
 * EXPECTED OUTPUT:
 *
 * === System Call Demonstration ===
 * PID: 12345
 *
 * [1] Creating and writing to file...
 * File opened for writing, fd=3
 * Wrote 44 bytes
 * File closed
 *
 * [2] Reading from file...
 * File opened for reading, fd=3
 * Read 44 bytes
 * Content:
 * Hello from system calls!
 * This is line 2.
 *
 * === File Information ===
 * File: test_syscall.txt
 * Size: 44 bytes
 * Inode: 1234567
 * Permissions: 644
 * Links: 1
 * UID: 1000, GID: 1000
 *
 * [3] Appending to file...
 * Appended 24 bytes
 *
 * [4] Reading entire file after append...
 * Content after append:
 * Hello from system calls!
 * This is line 2.
 * This line was appended!
 *
 * [5] Deleting test file...
 * File deleted successfully
 *
 * === Program Complete ===
 */

/*
 * EXERCISES:
 *
 * 1. Use strace to see all system calls:
 *    strace -e trace=open,read,write,close,stat,unlink ./basic_syscalls
 *
 * 2. Modify to use lseek() to read from middle of file
 *
 * 3. Add error recovery for partial writes
 *
 * 4. Implement a function to copy file using only system calls
 */
