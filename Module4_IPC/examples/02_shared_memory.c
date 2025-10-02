/*
 * 02_shared_memory.c - POSIX shared memory example
 * Demonstrates shared memory with semaphore synchronization
 *
 * Compile: gcc -o shm 02_shared_memory.c -lrt -lpthread
 * Run: ./shm writer  (in terminal 1)
 *      ./shm reader  (in terminal 2)
 */

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/my_shm"
#define SEM_NAME "/my_sem"
#define SHM_SIZE 4096

typedef struct {
  int counter;
  char message[256];
} shared_data_t;

void writer_process() {
  printf("=== Writer Process ===\n");

  // Create shared memory
  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  ftruncate(fd, SHM_SIZE);

  // Map shared memory
  shared_data_t *data =
      mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  // Create semaphore
  sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

  // Write data
  for (int i = 0; i < 5; i++) {
    sem_wait(sem);

    data->counter = i + 1;
    snprintf(data->message, sizeof(data->message), "Message #%d from writer",
             i + 1);

    printf("[Writer] Wrote: %s (counter=%d)\n", data->message, data->counter);

    sem_post(sem);
    sleep(1);
  }

  // Cleanup
  munmap(data, SHM_SIZE);
  close(fd);
  sem_close(sem);

  printf("[Writer] Done\n");
}

void reader_process() {
  printf("=== Reader Process ===\n");

  // Open shared memory
  int fd = shm_open(SHM_NAME, O_RDONLY, 0);
  if (fd == -1) {
    perror("shm_open");
    return;
  }

  // Map shared memory
  shared_data_t *data = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);

  // Open semaphore
  sem_t *sem = sem_open(SEM_NAME, 0);

  // Read data
  int last_counter = 0;
  for (int i = 0; i < 5; i++) {
    sleep(1);

    sem_wait(sem);

    if (data->counter != last_counter) {
      printf("[Reader] Read: %s (counter=%d)\n", data->message, data->counter);
      last_counter = data->counter;
    }

    sem_post(sem);
  }

  // Cleanup
  munmap(data, SHM_SIZE);
  close(fd);
  sem_close(sem);
  shm_unlink(SHM_NAME);
  sem_unlink(SEM_NAME);

  printf("[Reader] Done\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s [writer|reader]\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "writer") == 0) {
    writer_process();
  } else if (strcmp(argv[1], "reader") == 0) {
    reader_process();
  } else {
    printf("Invalid argument. Use 'writer' or 'reader'\n");
    return 1;
  }

  return 0;
}
