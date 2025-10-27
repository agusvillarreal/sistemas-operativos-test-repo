#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MSG_SIZE 100

struct message {
  long msg_type;
  char msg_text[MSG_SIZE];
};

int main(void) {
  key_t key;
  int msgid;
  struct message msg;

  printf("=== MESSAGE QUEUE RECEIVER ===\n");

  /* Generate same key as sender */
  key = ftok("/tmp", 'A');
  if (key == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  /* Access existing message queue */
  msgid = msgget(key, 0666 | IPC_CREAT);
  if (msgid == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  printf("Waiting for messages (Ctrl+C to exit)...\n\n");

  while (1) {
    /* Receive message of type 1 */
    if (msgrcv(msgid, &msg, MSG_SIZE, 1, 0) == -1) {
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }

    printf("📬 Received: %s\n", msg.msg_text);
  }

  return 0;
}

