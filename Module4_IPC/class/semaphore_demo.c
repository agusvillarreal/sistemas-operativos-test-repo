#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

/* semun is defined in sys/sem.h on macOS */
#if defined(__linux__)
union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};
#endif

int main(int argc, char *argv[]) {
  key_t key;
  int semid;
  union semun arg;
  struct sembuf sb;
  int process_id = 1;

  if (argc > 1) {
    process_id = atoi(argv[1]);
  }

  /* Generate key */
  key = ftok("/tmp", 'S');
  if (key == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  /* Create or access semaphore set with one semaphore */
  semid = semget(key, 1, 0666 | IPC_CREAT);
  if (semid == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  /* Initialize semaphore to 1 (binary semaphore/mutex) */
  arg.val = 1;
  semctl(semid, 0, SETVAL, arg);

  printf("[Process %d] Trying to enter critical section...\n", process_id);

  /* Wait operation (acquire lock) - BLOCKS if semaphore is 0 */
  sb.sem_num = 0;
  sb.sem_op = -1; /* decrement (wait/P operation) */
  sb.sem_flg = 0;

  if (semop(semid, &sb, 1) == -1) {
    perror("semop wait");
    exit(EXIT_FAILURE);
  }

  printf("[Process %d] ✓ ENTERED critical section (lock acquired)\n",
         process_id);
  printf("[Process %d] Working in critical section", process_id);
  fflush(stdout);

  /* Simulate work in critical section */
  for (int i = 0; i < 5; i++) {
    sleep(1);
    printf(".");
    fflush(stdout);
  }
  printf("\n");

  printf("[Process %d] ✓ LEAVING critical section (releasing lock)\n",
         process_id);

  /* Signal operation (release lock) */
  sb.sem_op = 1; /* increment (signal/V operation) */
  if (semop(semid, &sb, 1) == -1) {
    perror("semop signal");
    exit(EXIT_FAILURE);
  }

  printf("[Process %d] Done!\n", process_id);
  return 0;
}
