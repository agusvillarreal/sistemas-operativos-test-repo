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

  /* Generate unique key using file path and project id */
  key = ftok("/tmp", 'A');
  if (key == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  /* Create or access message queue */
  msgid = msgget(key, 0666 | IPC_CREAT);
  if (msgid == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  /* Prepare and send message */
  msg.msg_type = 1;
  strncpy(msg.msg_text, "Hello from sender", MSG_SIZE);

  if (msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0) == -1) {
    perror("msgsnd");
    exit(EXIT_FAILURE);
  }

  printf("Message sent to queue\n");
  return 0;
}