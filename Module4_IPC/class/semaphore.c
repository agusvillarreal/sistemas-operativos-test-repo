#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

/* semun is defined in sys/sem.h on macOS */
#if defined(__linux__)
union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};
#endif

int main(void) {
  key_t key;
  int semid;
  union semun arg;
  struct sembuf sb;

  key = ftok("/tmp", 'S');

  /* Create semaphore set with one semaphore */
  semid = semget(key, 1, 0666 | IPC_CREAT);
  if (semid == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  /* Initialize semaphore to 1 (binary semaphore) */
  arg.val = 1;
  if (semctl(semid, 0, SETVAL, arg) == -1) {
    perror("semctl");
    exit(EXIT_FAILURE);
  }

  /* Wait operation (acquire lock) */
  sb.sem_num = 0;
  sb.sem_op = -1; /* decrement */
  sb.sem_flg = 0;

  if (semop(semid, &sb, 1) == -1) {
    perror("semop wait");
    exit(EXIT_FAILURE);
  }

  printf("Critical section - only one process here at a time\n");

  /* Signal operation (release lock) */
  sb.sem_op = 1; /* increment */
  if (semop(semid, &sb, 1) == -1) {
    perror("semop signal");
    exit(EXIT_FAILURE);
  }

  return 0;
}