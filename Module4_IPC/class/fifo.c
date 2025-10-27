#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void) {
  const char *fifo_path = "/tmp/my_fifo";

  /* Create FIFO with read/write permissions for owner */
  if (mkfifo(fifo_path, 0666) == -1) {
    perror("mkfifo");
    return 1;
  }

  printf("FIFO created at %s\n", fifo_path);
  return 0;
}