# Module 4: Inter-Process Communication (IPC)
## Lecture Notes

---

## 📚 Table of Contents
1. [Introduction to IPC](#introduction)
2. [Pipes](#pipes)
3. [FIFOs (Named Pipes)](#fifos)
4. [Message Queues](#message-queues)
5. [Shared Memory](#shared-memory)
6. [Semaphores](#semaphores)
7. [UNIX Domain Sockets](#unix-sockets)
8. [Network Sockets](#network-sockets)
9. [Comparison of IPC Mechanisms](#comparison)

---

## 1. Introduction to IPC {#introduction}

### Why IPC?

Processes are isolated by design for security and stability. But they often need to **communicate** and **coordinate**.

### IPC Goals

1. **Data transfer** - Send data between processes
2. **Synchronization** - Coordinate execution
3. **Notification** - Signal events
4. **Resource sharing** - Share memory, files

### IPC Mechanisms in Linux

| Mechanism | Type | Use Case |
|-----------|------|----------|
| **Pipes** | Byte stream | Parent-child communication |
| **FIFOs** | Byte stream | Unrelated processes |
| **Message Queues** | Messages | Structured data exchange |
| **Shared Memory** | Memory | Fast, large data transfer |
| **Semaphores** | Synchronization | Mutual exclusion, signaling |
| **Sockets** | Byte stream | Network or local communication |
| **Signals** | Notification | Asynchronous events |

---

## 2. Pipes {#pipes}

### What is a Pipe?

A **pipe** is a unidirectional byte stream connecting two processes.

```
Process A (writer)  →  [ PIPE ]  →  Process B (reader)
```

### Creating a Pipe

```c
#include <unistd.h>

int pipe(int pipefd[2]);
// pipefd[0] = read end
// pipefd[1] = write end
```

### Basic Pipe Example

```c
int pipefd[2];
pipe(pipefd);

pid_t pid = fork();

if (pid == 0) {
    // Child: reader
    close(pipefd[1]);  // Close write end
    
    char buffer[100];
    int n = read(pipefd[0], buffer, sizeof(buffer));
    printf("Child read: %s\n", buffer);
    
    close(pipefd[0]);
    exit(0);
} else {
    // Parent: writer
    close(pipefd[0]);  // Close read end
    
    write(pipefd[1], "Hello from parent!", 18);
    
    close(pipefd[1]);
    wait(NULL);
}
```

### Pipe Characteristics

**Properties:**
- Unidirectional (one-way)
- Data is FIFO (first in, first out)
- Exists only while processes are alive
- Limited buffer size (typically 64KB)

**When to use:**
- Parent-child communication
- Simple data transfer
- Pipeline of commands (like shell pipes)

**Limitations:**
- Only related processes (must share file descriptors)
- Half-duplex (need 2 pipes for bidirectional)
- No message boundaries

### Pipe Capacity

```c
#include <fcntl.h>

// Get pipe capacity
int capacity = fcntl(pipefd[1], F_GETPIPE_SZ);
printf("Pipe capacity: %d bytes\n", capacity);

// Set pipe capacity (requires CAP_SYS_RESOURCE)
fcntl(pipefd[1], F_SETPIPE_SZ, 1024 * 1024);  // 1MB
```

### Blocking Behavior

**Read from empty pipe:** Blocks until data available  
**Write to full pipe:** Blocks until space available  
**Write to pipe with no readers:** SIGPIPE signal

```c
// Non-blocking I/O
int flags = fcntl(pipefd[0], F_GETFL);
fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

// Now read() returns -1 with errno=EAGAIN if no data
```

---

## 3. FIFOs (Named Pipes) {#fifos}

### What is a FIFO?

A **FIFO** (First In First Out) is a named pipe that exists in the filesystem.

**Difference from pipe:**
- Has a filename
- Can be used by unrelated processes
- Persists in filesystem

### Creating a FIFO

```c
#include <sys/stat.h>

// Create FIFO
int mkfifo(const char *pathname, mode_t mode);

// Example
mkfifo("/tmp/myfifo", 0666);
```

Or from command line:
```bash
mkfifo /tmp/myfifo
ls -l /tmp/myfifo
# prw-rw-rw- 1 user user 0 Oct 2 15:30 /tmp/myfifo
#  ↑ 'p' indicates pipe
```

### Using a FIFO

**Writer process:**
```c
int fd = open("/tmp/myfifo", O_WRONLY);
write(fd, "Hello", 5);
close(fd);
```

**Reader process:**
```c
int fd = open("/tmp/myfifo", O_RDONLY);
char buf[100];
int n = read(fd, buf, sizeof(buf));
close(fd);
```

### FIFO Blocking Behavior

```c
// Opening FIFO blocks until both ends are opened
fd = open("/tmp/myfifo", O_RDONLY);  // Blocks until writer opens

// Non-blocking open
fd = open("/tmp/myfifo", O_RDONLY | O_NONBLOCK);
// Returns immediately, even if no writer
```

### FIFO Example: Client-Server

**Server:**
```c
mkfifo("/tmp/server_fifo", 0666);

while (1) {
    int fd = open("/tmp/server_fifo", O_RDONLY);
    
    char request[256];
    read(fd, request, sizeof(request));
    
    printf("Received: %s\n", request);
    
    close(fd);
}
```

**Client:**
```c
int fd = open("/tmp/server_fifo", O_WRONLY);
write(fd, "Request from client", 20);
close(fd);
```

---

## 4. Message Queues {#message-queues}

### What is a Message Queue?

A **message queue** is a linked list of messages stored in the kernel.

**Advantages over pipes:**
- Message boundaries preserved
- Messages can have priorities
- Multiple readers/writers
- Random access by message type

### POSIX Message Queues

```c
#include <mqueue.h>

// Create/open message queue
mqd_t mq_open(const char *name, int oflag, mode_t mode, 
              struct mq_attr *attr);

// Send message
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, 
            unsigned int msg_prio);

// Receive message
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, 
                   unsigned int *msg_prio);

// Close
int mq_close(mqd_t mqdes);

// Remove
int mq_unlink(const char *name);
```

### Message Queue Example

**Create queue:**
```c
struct mq_attr attr;
attr.mq_flags = 0;
attr.mq_maxmsg = 10;      // Max messages in queue
attr.mq_msgsize = 256;    // Max message size
attr.mq_curmsgs = 0;

mqd_t mq = mq_open("/my_queue", O_CREAT | O_RDWR, 0644, &attr);
```

**Send message:**
```c
char msg[] = "Hello from sender";
mq_send(mq, msg, strlen(msg) + 1, 0);  // Priority 0
```

**Receive message:**
```c
char buffer[256];
unsigned int priority;
ssize_t bytes = mq_receive(mq, buffer, 256, &priority);

printf("Received (priority %u): %s\n", priority, buffer);
```

**Cleanup:**
```c
mq_close(mq);
mq_unlink("/my_queue");  // Remove from system
```

### Message Queue Attributes

```c
struct mq_attr {
    long mq_flags;      // Flags: 0 or O_NONBLOCK
    long mq_maxmsg;     // Max messages in queue
    long mq_msgsize;    // Max message size
    long mq_curmsgs;    // Current messages (read-only)
};

// Get attributes
struct mq_attr attr;
mq_getattr(mq, &attr);
printf("Messages in queue: %ld\n", attr.mq_curmsgs);
```

### Message Priorities

Higher priority messages are read first:

```c
// Send with different priorities
mq_send(mq, "Low priority", 13, 0);
mq_send(mq, "High priority", 14, 10);

// Receive gets high priority message first
mq_receive(mq, buffer, 256, &priority);
// Returns "High priority" (priority 10)
```

---

## 5. Shared Memory {#shared-memory}

### What is Shared Memory?

**Shared memory** allows multiple processes to access the same physical memory pages.

**Fastest IPC mechanism** - no kernel involvement for data transfer!

```
Process A         Process B
   │                 │
   └────► [Shared Memory] ◄────┘
```

### POSIX Shared Memory

```c
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Create/open shared memory object
int shm_open(const char *name, int oflag, mode_t mode);

// Set size
int ftruncate(int fd, off_t length);

// Map to address space
void *mmap(void *addr, size_t length, int prot, int flags, 
           int fd, off_t offset);

// Unmap
int munmap(void *addr, size_t length);

// Remove
int shm_unlink(const char *name);
```

### Shared Memory Example

**Writer process:**
```c
// Create shared memory
int fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);

// Set size to 4096 bytes
ftruncate(fd, 4096);

// Map into address space
char *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, 
                 MAP_SHARED, fd, 0);

// Write data
strcpy(ptr, "Hello from writer!");

// Unmap and close
munmap(ptr, 4096);
close(fd);
```

**Reader process:**
```c
// Open existing shared memory
int fd = shm_open("/my_shm", O_RDONLY, 0);

// Get size
struct stat sb;
fstat(fd, &sb);

// Map into address space
char *ptr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

// Read data
printf("Read: %s\n", ptr);

// Cleanup
munmap(ptr, sb.st_size);
close(fd);
shm_unlink("/my_shm");
```

### Synchronization Required!

**Problem:** Multiple processes accessing shared memory simultaneously → race conditions

**Solution:** Use semaphores or mutexes for synchronization

```c
// Shared data structure
typedef struct {
    sem_t mutex;
    int counter;
    char data[1000];
} shared_data_t;

// Writer
sem_wait(&shm->mutex);
shm->counter++;
strcpy(shm->data, "New data");
sem_post(&shm->mutex);

// Reader
sem_wait(&shm->mutex);
printf("Counter: %d, Data: %s\n", shm->counter, shm->data);
sem_post(&shm->mutex);
```

---

## 6. Semaphores {#semaphores}

### What is a Semaphore?

A **semaphore** is a kernel-maintained integer used for synchronization.

**Operations:**
- `sem_wait()` (P/down): Decrement and block if zero
- `sem_post()` (V/up): Increment and wake waiting process

### Types of Semaphores

**1. Binary Semaphore (Mutex)**
- Values: 0 or 1
- Use: Mutual exclusion

**2. Counting Semaphore**
- Values: 0 to N
- Use: Resource counting, producer-consumer

### POSIX Semaphores

```c
#include <semaphore.h>

// Named semaphore (can be used by unrelated processes)
sem_t *sem_open(const char *name, int oflag, mode_t mode, 
                unsigned int value);
int sem_close(sem_t *sem);
int sem_unlink(const char *name);

// Unnamed semaphore (in shared memory)
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);

// Operations
int sem_wait(sem_t *sem);      // Decrement (blocks if 0)
int sem_trywait(sem_t *sem);   // Non-blocking wait
int sem_post(sem_t *sem);      // Increment
int sem_getvalue(sem_t *sem, int *sval);
```

### Mutex Example

```c
// Create named semaphore (initial value 1 = unlocked)
sem_t *mutex = sem_open("/my_mutex", O_CREAT, 0644, 1);

// Critical section
sem_wait(mutex);
// ... access shared resource ...
sem_post(mutex);

// Cleanup
sem_close(mutex);
sem_unlink("/my_mutex");
```

### Producer-Consumer Example

```c
#define BUFFER_SIZE 10

sem_t *empty;  // Count of empty slots
sem_t *full;   // Count of full slots
sem_t *mutex;  // Mutual exclusion

// Initialize
empty = sem_open("/empty", O_CREAT, 0644, BUFFER_SIZE);
full = sem_open("/full", O_CREAT, 0644, 0);
mutex = sem_open("/mutex", O_CREAT, 0644, 1);

// Producer
void produce(int item) {
    sem_wait(empty);  // Wait for empty slot
    sem_wait(mutex);  // Enter critical section
    
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    
    sem_post(mutex);  // Leave critical section
    sem_post(full);   // Increment full count
}

// Consumer
int consume() {
    sem_wait(full);   // Wait for full slot
    sem_wait(mutex);  // Enter critical section
    
    int item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    
    sem_post(mutex);  // Leave critical section
    sem_post(empty);  // Increment empty count
    
    return item;
}
```

---

## 7. UNIX Domain Sockets {#unix-sockets}

### What are UNIX Domain Sockets?

**UNIX domain sockets** are like network sockets but for local communication.

**Advantages:**
- Bidirectional
- Stream or datagram
- File descriptor passing
- Faster than TCP/IP loopback

### Socket Types

**SOCK_STREAM:** Connection-oriented (like TCP)  
**SOCK_DGRAM:** Connectionless (like UDP)

### Stream Socket Example

**Server:**
```c
#include <sys/socket.h>
#include <sys/un.h>

// Create socket
int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

// Bind to address
struct sockaddr_un addr;
addr.sun_family = AF_UNIX;
strcpy(addr.sun_path, "/tmp/my_socket");

unlink("/tmp/my_socket");  // Remove if exists
bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

// Listen
listen(server_fd, 5);

// Accept connection
int client_fd = accept(server_fd, NULL, NULL);

// Communicate
char buf[256];
read(client_fd, buf, sizeof(buf));
write(client_fd, "Response", 8);

close(client_fd);
close(server_fd);
unlink("/tmp/my_socket");
```

**Client:**
```c
// Create socket
int sock = socket(AF_UNIX, SOCK_STREAM, 0);

// Connect to server
struct sockaddr_un addr;
addr.sun_family = AF_UNIX;
strcpy(addr.sun_path, "/tmp/my_socket");

connect(sock, (struct sockaddr*)&addr, sizeof(addr));

// Communicate
write(sock, "Request", 7);
char buf[256];
read(sock, buf, sizeof(buf));

close(sock);
```

---

## 8. Network Sockets {#network-sockets}

### TCP Socket Example

**Server:**
```c
// Create socket
int server_fd = socket(AF_INET, SOCK_STREAM, 0);

// Bind to port
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = INADDR_ANY;
addr.sin_port = htons(8888);

bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

// Listen
listen(server_fd, 5);

// Accept
int client_fd = accept(server_fd, NULL, NULL);

// Communicate
char buf[256];
recv(client_fd, buf, sizeof(buf), 0);
send(client_fd, "OK", 2, 0);

close(client_fd);
close(server_fd);
```

**Client:**
```c
// Create socket
int sock = socket(AF_INET, SOCK_STREAM, 0);

// Connect
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(8888);
inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

connect(sock, (struct sockaddr*)&addr, sizeof(addr));

// Communicate
send(sock, "Hello", 5, 0);
char buf[256];
recv(sock, buf, sizeof(buf), 0);

close(sock);
```

---

## 9. Comparison of IPC Mechanisms {#comparison}

### Performance Comparison

| Mechanism | Speed | Complexity | Use Case |
|-----------|-------|------------|----------|
| **Shared Memory** | Fastest | High | Large data, high frequency |
| **Pipes** | Fast | Low | Parent-child, simple |
| **UNIX Sockets** | Fast | Medium | Local client-server |
| **Message Queues** | Medium | Medium | Structured messages |
| **Network Sockets** | Slow | High | Network communication |

### Feature Comparison

| Feature | Pipes | FIFOs | Msg Queue | Shared Mem | Sockets |
|---------|-------|-------|-----------|------------|---------|
| Bidirectional | No¹ | No¹ | Yes | Yes | Yes |
| Message boundaries | No | No | Yes | No² | Datagram only |
| Multiple readers | No | Yes | Yes | Yes | Stream: No, Dgram: Yes |
| Persistent | No | Yes³ | Yes | Yes | Yes³ |
| Network capable | No | No | No | No | Yes |

¹ Need 2 pipes/FIFOs for bidirectional  
² Application must add framing  
³ FIFO/socket file persists, but not data

### Choosing the Right IPC

**Use Pipes when:**
- Parent-child communication
- Simple one-way data flow
- Command pipelines

**Use FIFOs when:**
- Unrelated processes
- Named communication channel
- Simple setup

**Use Message Queues when:**
- Message-based protocol
- Need priorities
- Multiple producers/consumers

**Use Shared Memory when:**
- Large data transfer
- High-frequency updates
- Need maximum speed

**Use Semaphores when:**
- Synchronization needed
- Mutual exclusion
- Resource counting

**Use Sockets when:**
- Network communication
- Flexible client-server
- Standard protocols (HTTP, etc.)

---

## 📝 Summary

### Key Takeaways

1. **Pipes:** Simple, unidirectional, parent-child
2. **FIFOs:** Named pipes, unrelated processes
3. **Message Queues:** Structured messages, priorities
4. **Shared Memory:** Fastest, requires synchronization
5. **Semaphores:** Synchronization primitive
6. **Sockets:** Flexible, bidirectional, network-capable

### Best Practices

- **Always check return values** of IPC system calls
- **Close file descriptors** when done
- **Unlink named objects** (FIFOs, shared memory, semaphores)
- **Use synchronization** with shared memory
- **Handle EINTR** (interrupted system calls)
- **Consider security** - permissions on IPC objects

---

**Next Module:** Kernel Modules and Device Drivers

