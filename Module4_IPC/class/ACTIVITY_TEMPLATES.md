# Activity Code Templates

This file provides starter code templates for common IPC activities. Use these as a foundation and build upon them!

---

## Template 1: Bidirectional Pipe Communication

**Activity 1.1 Starter Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
  int pipe1[2], pipe2[2];  // pipe1: parent->child, pipe2: child->parent
  pid_t pid;
  char parent_msg[] = "Hello from parent";
  char child_msg[] = "Hello from child";
  char buffer[100];
  
  // TODO: Create both pipes
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  
  pid = fork();
  
  if (pid == 0) {
    /* CHILD PROCESS */
    // TODO: Close unused pipe ends
    // close(pipe1[1]);  // Close write end of pipe1
    // close(pipe2[0]);  // Close read end of pipe2
    
    // TODO: Read from parent via pipe1
    
    // TODO: Send message to parent via pipe2
    
    // TODO: Close remaining pipe ends
    exit(EXIT_SUCCESS);
  } else {
    /* PARENT PROCESS */
    // TODO: Close unused pipe ends
    
    // TODO: Send message to child via pipe1
    
    // TODO: Read from child via pipe2
    
    // TODO: Close remaining pipe ends
    wait(NULL);
  }
  
  return 0;
}
```

---

## Template 2: Pipe Calculator

**Activity 1.2 Starter Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
  int req_pipe[2], res_pipe[2];
  pid_t pid;
  
  if (pipe(req_pipe) == -1 || pipe(res_pipe) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  
  pid = fork();
  
  if (pid == 0) {
    /* CHILD - Calculator */
    close(req_pipe[1]);
    close(res_pipe[0]);
    
    while (1) {
      char expression[100];
      ssize_t n = read(req_pipe[0], expression, sizeof(expression));
      if (n <= 0) break;
      
      // TODO: Parse expression (e.g., "5 + 3")
      int num1, num2, result;
      char op;
      
      if (sscanf(expression, "%d %c %d", &num1, &op, &num2) == 3) {
        // TODO: Calculate based on operator
        switch(op) {
          case '+': result = num1 + num2; break;
          case '-': result = num1 - num2; break;
          case '*': result = num1 * num2; break;
          case '/': 
            if (num2 != 0) result = num1 / num2;
            else result = 0;  // Error case
            break;
          default: result = 0;
        }
        
        // TODO: Send result back to parent
        write(res_pipe[1], &result, sizeof(result));
      }
    }
    
    close(req_pipe[0]);
    close(res_pipe[1]);
    exit(EXIT_SUCCESS);
    
  } else {
    /* PARENT - User Interface */
    close(req_pipe[0]);
    close(res_pipe[1]);
    
    // TODO: Get expressions from user and send to child
    char expression[100];
    printf("Enter expression (e.g., '5 + 3') or 'quit':\n");
    
    while (fgets(expression, sizeof(expression), stdin)) {
      if (strncmp(expression, "quit", 4) == 0) break;
      
      // Send to child
      write(req_pipe[1], expression, strlen(expression) + 1);
      
      // Get result
      int result;
      if (read(res_pipe[0], &result, sizeof(result)) > 0) {
        printf("Result: %d\n", result);
      }
    }
    
    close(req_pipe[1]);
    close(res_pipe[0]);
    wait(NULL);
  }
  
  return 0;
}
```

---

## Template 3: Chat Application with Timestamps

**Activity 2.1 Starter Code - Writer**

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(void) {
  const char *fifo_path = "/tmp/my_fifo";
  int fd;
  char username[50];
  char message[200];
  char formatted_msg[300];
  
  // TODO: Get username
  printf("Enter your username: ");
  fgets(username, sizeof(username), stdin);
  username[strcspn(username, "\n")] = 0;  // Remove newline
  
  fd = open(fifo_path, O_WRONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }
  
  printf("Chat started! Type messages (or 'quit' to exit):\n");
  
  while (1) {
    printf("> ");
    if (fgets(message, sizeof(message), stdin) == NULL) break;
    
    message[strcspn(message, "\n")] = 0;
    if (strcmp(message, "quit") == 0) break;
    
    // TODO: Add timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    // Format: [HH:MM:SS] [Username]: Message
    snprintf(formatted_msg, sizeof(formatted_msg),
             "[%02d:%02d:%02d] [%s]: %s",
             t->tm_hour, t->tm_min, t->tm_sec,
             username, message);
    
    write(fd, formatted_msg, strlen(formatted_msg) + 1);
  }
  
  close(fd);
  return 0;
}
```

---

## Template 4: Priority Message System

**Activity 4.1 Starter Code - Sender**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100

struct message {
  long msg_type;  // 1=LOW, 2=NORMAL, 3=HIGH, 4=CRITICAL
  char msg_text[MSG_SIZE];
};

int main(void) {
  key_t key = ftok("/tmp", 'P');
  int msgid = msgget(key, 0666 | IPC_CREAT);
  
  if (msgid == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }
  
  struct message msg;
  char input[MSG_SIZE];
  int priority;
  
  printf("Priority Message Sender\n");
  printf("Priorities: 1=LOW, 2=NORMAL, 3=HIGH, 4=CRITICAL\n\n");
  
  while (1) {
    printf("Enter priority (1-4, 0 to quit): ");
    scanf("%d", &priority);
    getchar();  // Consume newline
    
    if (priority == 0) break;
    if (priority < 1 || priority > 4) {
      printf("Invalid priority!\n");
      continue;
    }
    
    printf("Enter message: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    // TODO: Set message type and send
    msg.msg_type = priority;
    strncpy(msg.msg_text, input, MSG_SIZE);
    
    if (msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0) == -1) {
      perror("msgsnd");
    } else {
      const char *level[] = {"", "LOW", "NORMAL", "HIGH", "CRITICAL"};
      printf("✓ Sent with priority: %s\n\n", level[priority]);
    }
  }
  
  return 0;
}
```

**Activity 4.1 Starter Code - Receiver**

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100

struct message {
  long msg_type;
  char msg_text[MSG_SIZE];
};

int main(void) {
  key_t key = ftok("/tmp", 'P');
  int msgid = msgget(key, 0666 | IPC_CREAT);
  
  struct message msg;
  const char *level[] = {"", "LOW", "NORMAL", "HIGH", "CRITICAL"};
  
  printf("Priority Message Receiver\n");
  printf("Receiving messages in priority order...\n\n");
  
  while (1) {
    // TODO: Receive messages by priority
    // Try CRITICAL first (type=4), then HIGH (type=3), etc.
    
    for (int priority = 4; priority >= 1; priority--) {
      // IPC_NOWAIT: Don't block if no message of this type
      if (msgrcv(msgid, &msg, MSG_SIZE, priority, IPC_NOWAIT) > 0) {
        printf("[%s] %s\n", level[priority], msg.msg_text);
        goto next_iteration;  // Process one message at a time
      }
    }
    
    // If no priority messages, try any message
    if (msgrcv(msgid, &msg, MSG_SIZE, 0, IPC_NOWAIT) > 0) {
      printf("[%s] %s\n", level[msg.msg_type], msg.msg_text);
    } else {
      // No messages available, wait a bit
      usleep(100000);  // 0.1 seconds
    }
    
    next_iteration:
    continue;
  }
  
  return 0;
}
```

---

## Template 5: Bank Account Synchronization

**Activity 5.1 Starter Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#if defined(__linux__)
union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};
#endif

// Shared memory structure
struct shared_data {
  int balance;
};

void sem_wait(int semid) {
  struct sembuf sb = {0, -1, 0};
  semop(semid, &sb, 1);
}

void sem_signal(int semid) {
  struct sembuf sb = {0, 1, 0};
  semop(semid, &sb, 1);
}

void deposit(struct shared_data *data, int semid, int amount, int use_semaphore) {
  if (use_semaphore) sem_wait(semid);
  
  // CRITICAL SECTION
  int temp = data->balance;
  usleep(100);  // Simulate processing time - increases chance of race condition
  temp += amount;
  data->balance = temp;
  
  if (use_semaphore) sem_signal(semid);
}

void withdraw(struct shared_data *data, int semid, int amount, int use_semaphore) {
  if (use_semaphore) sem_wait(semid);
  
  // CRITICAL SECTION
  int temp = data->balance;
  usleep(100);
  temp -= amount;
  data->balance = temp;
  
  if (use_semaphore) sem_signal(semid);
}

int main(int argc, char *argv[]) {
  int use_semaphore = (argc > 1 && strcmp(argv[1], "safe") == 0);
  
  // TODO: Create shared memory
  key_t shm_key = ftok("/tmp", 'B');
  int shmid = shmget(shm_key, sizeof(struct shared_data), IPC_CREAT | 0666);
  struct shared_data *data = shmat(shmid, NULL, 0);
  data->balance = 1000;
  
  // TODO: Create semaphore
  key_t sem_key = ftok("/tmp", 'S');
  int semid = semget(sem_key, 1, IPC_CREAT | 0666);
  union semun arg;
  arg.val = 1;
  semctl(semid, 0, SETVAL, arg);
  
  printf("Initial balance: $%d\n", data->balance);
  printf("Running %s version\n", use_semaphore ? "SAFE (with semaphore)" : "UNSAFE (no semaphore)");
  
  // TODO: Fork multiple processes
  // 10 processes deposit $100
  // 10 processes withdraw $50
  // Expected final: 1000 + 1000 - 500 = 1500
  
  for (int i = 0; i < 10; i++) {
    if (fork() == 0) {
      deposit(data, semid, 100, use_semaphore);
      exit(0);
    }
  }
  
  for (int i = 0; i < 10; i++) {
    if (fork() == 0) {
      withdraw(data, semid, 50, use_semaphore);
      exit(0);
    }
  }
  
  // Wait for all children
  for (int i = 0; i < 20; i++) {
    wait(NULL);
  }
  
  printf("Final balance: $%d\n", data->balance);
  printf("Expected: $1500\n");
  
  if (data->balance == 1500) {
    printf("✓ CORRECT!\n");
  } else {
    printf("✗ WRONG! Race condition occurred.\n");
  }
  
  // Cleanup
  shmdt(data);
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 0, IPC_RMID);
  
  return 0;
}

/* Compile: gcc -o bank_account bank_account.c
 * Run unsafe: ./bank_account
 * Run safe:   ./bank_account safe
 */
```

---

## Template 6: Reader-Writer Problem

**Activity 5.2 Starter Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#if defined(__linux__)
union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};
#endif

// Semaphore operations
void sem_wait(int semid, int sem_num) {
  struct sembuf sb = {sem_num, -1, 0};
  semop(semid, &sb, 1);
}

void sem_signal(int semid, int sem_num) {
  struct sembuf sb = {sem_num, 1, 0};
  semop(semid, &sb, 1);
}

#define SEM_MUTEX 0
#define SEM_WRITE 1

struct shared_data {
  int reader_count;
  int data_value;
};

void reader(int id, struct shared_data *shared, int semid) {
  // TODO: Implement reader logic
  
  // Entry section
  sem_wait(semid, SEM_MUTEX);
  shared->reader_count++;
  if (shared->reader_count == 1) {
    sem_wait(semid, SEM_WRITE);  // First reader blocks writers
  }
  sem_signal(semid, SEM_MUTEX);
  
  // Critical section - reading
  printf("[Reader %d] Reading value: %d\n", id, shared->data_value);
  sleep(1);  // Simulate reading time
  
  // Exit section
  sem_wait(semid, SEM_MUTEX);
  shared->reader_count--;
  if (shared->reader_count == 0) {
    sem_signal(semid, SEM_WRITE);  // Last reader unblocks writers
  }
  sem_signal(semid, SEM_MUTEX);
}

void writer(int id, struct shared_data *shared, int semid) {
  // TODO: Implement writer logic
  
  sem_wait(semid, SEM_WRITE);
  
  // Critical section - writing
  printf("[Writer %d] Writing new value...\n", id);
  shared->data_value = id * 100;
  sleep(2);  // Simulate writing time
  printf("[Writer %d] Wrote value: %d\n", id, shared->data_value);
  
  sem_signal(semid, SEM_WRITE);
}

int main(void) {
  // TODO: Setup shared memory
  key_t shm_key = ftok("/tmp", 'R');
  int shmid = shmget(shm_key, sizeof(struct shared_data), IPC_CREAT | 0666);
  struct shared_data *shared = shmat(shmid, NULL, 0);
  shared->reader_count = 0;
  shared->data_value = 42;
  
  // TODO: Setup semaphores (need 2: mutex and write)
  key_t sem_key = ftok("/tmp", 'W');
  int semid = semget(sem_key, 2, IPC_CREAT | 0666);
  
  union semun arg;
  arg.val = 1;
  semctl(semid, SEM_MUTEX, SETVAL, arg);
  semctl(semid, SEM_WRITE, SETVAL, arg);
  
  // TODO: Fork readers and writers
  // Create 5 readers
  for (int i = 1; i <= 5; i++) {
    if (fork() == 0) {
      reader(i, shared, semid);
      exit(0);
    }
  }
  
  // Create 2 writers
  for (int i = 1; i <= 2; i++) {
    if (fork() == 0) {
      writer(i, shared, semid);
      exit(0);
    }
  }
  
  // Wait for all
  for (int i = 0; i < 7; i++) {
    wait(NULL);
  }
  
  printf("\nFinal value: %d\n", shared->data_value);
  
  // Cleanup
  shmdt(shared);
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 0, IPC_RMID);
  
  return 0;
}
```

---

## Template 7: Job Queue System

**Activity 4.3 Starter Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSG_SIZE 200

struct job_message {
  long msg_type;  // 1 = job request, 2 = job result
  char job_data[MSG_SIZE];
};

// Job format: "COMPUTE:factorial:10"
long long factorial(int n) {
  if (n <= 1) return 1;
  return n * factorial(n - 1);
}

void worker(int worker_id, int msgid) {
  struct job_message job, result;
  
  printf("[Worker %d] Started and waiting for jobs...\n", worker_id);
  
  while (1) {
    // TODO: Receive job from queue (type=1)
    if (msgrcv(msgid, &job, MSG_SIZE, 1, 0) == -1) {
      perror("msgrcv");
      break;
    }
    
    printf("[Worker %d] Received job: %s\n", worker_id, job.job_data);
    
    // TODO: Parse and process job
    char operation[50], function[50];
    int value;
    
    if (sscanf(job.job_data, "%[^:]:%[^:]:%d", operation, function, &value) == 3) {
      if (strcmp(operation, "COMPUTE") == 0) {
        if (strcmp(function, "factorial") == 0) {
          long long res = factorial(value);
          snprintf(result.job_data, MSG_SIZE, 
                   "Worker %d: factorial(%d) = %lld", worker_id, value, res);
        }
      }
    } else {
      snprintf(result.job_data, MSG_SIZE, 
               "Worker %d: Invalid job format", worker_id);
    }
    
    // TODO: Send result (type=2)
    result.msg_type = 2;
    msgsnd(msgid, &result, strlen(result.job_data) + 1, 0);
    
    printf("[Worker %d] Completed job\n", worker_id);
    sleep(1);  // Simulate processing time
  }
}

void producer(int producer_id, int msgid, int num_jobs) {
  struct job_message job;
  job.msg_type = 1;
  
  printf("[Producer %d] Submitting %d jobs...\n", producer_id, num_jobs);
  
  for (int i = 0; i < num_jobs; i++) {
    // TODO: Create different types of jobs
    int value = (producer_id * 10) + i;
    snprintf(job.job_data, MSG_SIZE, "COMPUTE:factorial:%d", value);
    
    msgsnd(msgid, &job, strlen(job.job_data) + 1, 0);
    printf("[Producer %d] Submitted: %s\n", producer_id, job.job_data);
    sleep(1);
  }
}

void result_collector(int msgid, int expected_results) {
  struct job_message result;
  int received = 0;
  
  printf("[Collector] Waiting for %d results...\n", expected_results);
  
  while (received < expected_results) {
    // TODO: Receive results (type=2)
    if (msgrcv(msgid, &result, MSG_SIZE, 2, 0) > 0) {
      printf("[RESULT %d] %s\n", ++received, result.job_data);
    }
  }
  
  printf("[Collector] All results received!\n");
}

int main(void) {
  key_t key = ftok("/tmp", 'J');
  int msgid = msgget(key, 0666 | IPC_CREAT);
  
  int num_producers = 3;
  int num_workers = 5;
  int jobs_per_producer = 4;
  int total_jobs = num_producers * jobs_per_producer;
  
  // TODO: Fork workers
  for (int i = 1; i <= num_workers; i++) {
    if (fork() == 0) {
      worker(i, msgid);
      exit(0);
    }
  }
  
  sleep(1);  // Let workers start
  
  // TODO: Fork producers
  for (int i = 1; i <= num_producers; i++) {
    if (fork() == 0) {
      producer(i, msgid, jobs_per_producer);
      exit(0);
    }
  }
  
  // TODO: Collect results (in parent)
  result_collector(msgid, total_jobs);
  
  // Wait for all children
  for (int i = 0; i < num_producers + num_workers; i++) {
    wait(NULL);
  }
  
  // Cleanup
  msgctl(msgid, IPC_RMID, NULL);
  
  return 0;
}
```

---

## 💡 Usage Tips

1. **Copy the template** to a new file
2. **Read all the TODOs** carefully
3. **Implement step by step**, testing as you go
4. **Remove TODOs** when completed
5. **Add your own comments** explaining your code

## 🧪 Testing Your Code

Always test with:
```bash
# Compile with warnings
gcc -Wall -Wextra -g -o myprogram myprogram.c

# Run
./myprogram

# Check for memory leaks
valgrind --leak-check=full ./myprogram

# Check IPC resources
ipcs -a
```

## 🎯 Next Steps

After completing a template:
1. Test thoroughly
2. Add error handling
3. Add features/extensions
4. Document your code
5. Help classmates!

---

**Good luck with your IPC activities!** 🚀

