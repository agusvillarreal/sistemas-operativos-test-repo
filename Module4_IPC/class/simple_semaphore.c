#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Simple semaphore structure */
struct semaphore {
  int count; // number of available resources
  // In real implementation, would also have:
  // queue q; // waiting processes queue
};

/* Wait operation (P operation, down, acquire) */
void sem_wait(struct semaphore *s) {
  s->count--; // decrement

  if (s->count < 0) {
    printf("  [Process %d] count=%d, BLOCKED (added to queue)\n", getpid(),
           s->count);
    // In real OS: remove process from ready queue
    // and put it in semaphore's waiting queue
    sleep(2); // Simulate blocking
  } else {
    printf("  [Process %d] count=%d, proceeding\n", getpid(), s->count);
  }
}

/* Signal operation (V operation, up, release) */
void sem_signal(struct semaphore *s) {
  s->count++; // increment

  if (s->count <= 0) {
    printf("  [Process %d] count=%d, waking up one waiting process\n", getpid(),
           s->count);
    // In real OS: remove one process from waiting queue
    // and add it to ready queue
  } else {
    printf("  [Process %d] count=%d, no process waiting\n", getpid(), s->count);
  }
}

/* Worker process function */
void worker(struct semaphore *sem, int id) {
  printf("\n[Worker %d] Starting (PID: %d)\n", id, getpid());
  sleep(id - 1); // Stagger processes

  printf("[Worker %d] Calling sem_wait()...\n", id);
  sem_wait(sem);

  // === CRITICAL SECTION ===
  printf("\n>>> [Worker %d] IN CRITICAL SECTION <<<\n", id);
  printf(">>> [Worker %d] Working", id);
  fflush(stdout);
  for (int i = 0; i < 3; i++) {
    sleep(1);
    printf(".");
    fflush(stdout);
  }
  printf(" Done!\n");
  printf(">>> [Worker %d] LEAVING CRITICAL SECTION <<<\n\n", id);
  // === END CRITICAL SECTION ===

  printf("[Worker %d] Calling sem_signal()...\n", id);
  sem_signal(sem);

  printf("[Worker %d] Exiting (PID: %d)\n", id, getpid());
}

int main(void) {
  int shmid;
  struct semaphore *sem;
  pid_t pids[3];

  printf("===================================\n");
  printf("Simple Semaphore Implementation\n");
  printf("===================================\n\n");

  // Create shared memory for semaphore
  shmid = shmget(IPC_PRIVATE, sizeof(struct semaphore), IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(1);
  }

  // Attach shared memory
  sem = (struct semaphore *)shmat(shmid, NULL, 0);
  if (sem == (void *)-1) {
    perror("shmat");
    exit(1);
  }

  // Initialize semaphore
  printf("Initializing semaphore with count = 1 (binary semaphore/mutex)\n");
  sem->count = 1;

  printf("\nSemaphore Structure:\n");
  printf("  struct semaphore {\n");
  printf("    int count = %d;  // available resources\n", sem->count);
  printf("    queue q;         // waiting processes (not shown)\n");
  printf("  };\n\n");

  printf("Creating 3 worker processes...\n");

  // Create 3 child processes
  for (int i = 0; i < 3; i++) {
    pids[i] = fork();

    if (pids[i] == -1) {
      perror("fork");
      exit(1);
    } else if (pids[i] == 0) {
      // Child process
      worker(sem, i + 1);
      exit(0);
    }
    // Parent continues to fork more children
  }

  // Parent waits for all children
  for (int i = 0; i < 3; i++) {
    waitpid(pids[i], NULL, 0);
  }

  // Cleanup
  shmdt(sem);
  shmctl(shmid, IPC_RMID, NULL);

  printf("\n===================================\n");
  printf("All processes finished!\n");
  printf("Notice: Only ONE process in critical section at a time!\n");
  printf("===================================\n");

  return 0;
}
