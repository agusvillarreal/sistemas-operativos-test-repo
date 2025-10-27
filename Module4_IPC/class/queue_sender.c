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
  char input[MSG_SIZE];

  printf("=== MESSAGE QUEUE SENDER ===\n");

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

  printf("Enter messages to send (type 'quit' to exit):\n");

  while (1) {
    printf("> ");
    if (fgets(input, MSG_SIZE, stdin) == NULL) {
      break;
    }

    /* Remove newline */
    input[strcspn(input, "\n")] = 0;

    /* Check for quit */
    if (strcmp(input, "quit") == 0) {
      break;
    }

    /* Prepare and send message */
    msg.msg_type = 1;
    strncpy(msg.msg_text, input, MSG_SIZE - 1);
    msg.msg_text[MSG_SIZE - 1] = '\0';

    if (msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0) == -1) {
      perror("msgsnd");
      exit(EXIT_FAILURE);
    }

    printf("✓ Message sent to queue\n");
  }

  printf("Sender exiting.\n");
  return 0;
}

