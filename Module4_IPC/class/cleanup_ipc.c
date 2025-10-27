#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

int main(void) {
  key_t sem_key, msg_key;
  int semid, msgid;

  printf("=== IPC Cleanup Utility ===\n");

  /* Clean up semaphore */
  sem_key = ftok("/tmp", 'S');
  if (sem_key != -1) {
    semid = semget(sem_key, 1, 0666);
    if (semid != -1) {
      if (semctl(semid, 0, IPC_RMID) == 0) {
        printf("✓ Semaphore removed\n");
      } else {
        printf("✗ Failed to remove semaphore\n");
      }
    } else {
      printf("- No semaphore to remove\n");
    }
  }

  /* Clean up message queue */
  msg_key = ftok("/tmp", 'A');
  if (msg_key != -1) {
    msgid = msgget(msg_key, 0666);
    if (msgid != -1) {
      if (msgctl(msgid, IPC_RMID, NULL) == 0) {
        printf("✓ Message queue removed\n");
      } else {
        printf("✗ Failed to remove message queue\n");
      }
    } else {
      printf("- No message queue to remove\n");
    }
  }

  printf("\nCleanup complete!\n");
  return 0;
}

