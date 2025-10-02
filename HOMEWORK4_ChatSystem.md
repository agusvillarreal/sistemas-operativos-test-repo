# Homework 4: Chat System using IPC
## Standalone Assignment - Inter-Process Communication

**Due Date:** 3 weeks from assignment date  
**Weight:** 20% of final grade  
**Points:** 100 points  
**Submission:** Source code + README + Makefile + demo video

---

## 📌 Assignment Overview

This is **Homework 4 of 5** in the Linux Systems Programming course. Each homework is an **independent project**:

- Homework 1: Basic Process Management
- Homework 2: Zombie Process Handler
- Homework 3: Scheduler Simulator with GUI
- **Homework 4** (This): Chat System using IPC ← You are here
- Homework 5: Custom Kernel Module

---

## 🎯 Learning Objectives

By completing this homework, you will:
1. Implement multi-client server architecture
2. Master IPC mechanisms (choose 2 of: pipes, message queues, shared memory, sockets)
3. Handle synchronization with semaphores/mutexes
4. Build robust client-server communication
5. Handle concurrent clients and edge cases

---

## 📋 Assignment Description

You will implement a **Multi-Client Chat System** that allows users to:
- Connect multiple clients to a central server
- Send messages to all users (broadcast)
- Send private messages to specific users
- See list of online users
- Receive notifications (joins, leaves)
- Handle disconnections gracefully

**Architecture:**
```
Client 1 ─┐
Client 2 ─┼──> SERVER ──> Manages messages
Client 3 ─┤              Broadcasts to all
Client 4 ─┘              Handles users
```

---

## 🔧 Requirements

### Part 1: Server Implementation (35 points)

Implement `chat_server.c`:

```c
// Server must support:
- Accept multiple client connections
- Maintain list of connected clients
- Receive messages from any client
- Broadcast messages to all clients
- Handle private messages
- Detect and remove disconnected clients
- Log all activity
```

**Server Commands:**
```
/list              - Show all connected users
/stats             - Show server statistics
/kick <username>   - Disconnect a user
/shutdown          - Graceful server shutdown
/broadcast <msg>   - Send message from server
```

**Server Output:**
```
[SERVER] Chat server started on port 8888
[SERVER] Using IPC method: UNIX_SOCKETS
[INFO] Client 'alice' connected (FD: 4)
[INFO] Client 'bob' connected (FD: 5)
[MSG] alice -> ALL: Hello everyone!
[MSG] alice -> bob: Hi Bob!
[INFO] Client 'alice' disconnected
[INFO] Total messages: 2, Active users: 1
```

---

### Part 2: Client Implementation (25 points)

Implement `chat_client.c`:

```c
// Client must support:
- Connect to server
- Set username
- Send messages
- Receive messages
- Display user list
- Handle server disconnect
```

**Client Commands:**
```
/help              - Show commands
/users             - List online users
/msg <user> <msg>  - Send private message
/quit              - Disconnect
```

**Client Interface:**
```
=== Chat Client ===
Username: alice
Connected to server!

[SYSTEM] Welcome alice!
[SYSTEM] Users online: bob, charlie

alice> Hello everyone!
[bob] Hi alice!
[alice -> you] Check your messages

alice> /msg bob Got it!
[You -> bob] Got it!

alice> /quit
[SYSTEM] Goodbye!
```

---

### Part 3: IPC Implementation (30 points)

Choose **TWO** of the following IPC mechanisms:

#### Option 1: UNIX Domain Sockets (Recommended)

```c
// Server
int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
struct sockaddr_un addr;
addr.sun_family = AF_UNIX;
strcpy(addr.sun_path, "/tmp/chat.sock");
bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
listen(server_fd, 10);

// Client
connect(client_fd, (struct sockaddr*)&addr, sizeof(addr));
```

**Pros:** Most versatile, handles multiple clients easily  
**Cons:** More complex

#### Option 2: Named Pipes (FIFOs)

```c
// Create FIFOs
mkfifo("/tmp/chat_to_server", 0666);
mkfifo("/tmp/chat_to_client1", 0666);

// Server reads from chat_to_server
// Clients write to chat_to_server
// Server writes to individual client FIFOs
```

**Pros:** Simple, Unix-like  
**Cons:** One FIFO per client needed

#### Option 3: Message Queues (POSIX)

```c
mqd_t mq = mq_open("/chat_queue", O_CREAT | O_RDWR, 0644, &attr);

struct message {
    long type;          // 1=broadcast, 2=private, 3=control
    int from_pid;
    int to_pid;
    char username[32];
    char text[256];
};

mq_send(mq, (char*)&msg, sizeof(msg), 0);
mq_receive(mq, (char*)&msg, sizeof(msg), NULL);
```

**Pros:** Built-in message boundaries, priority  
**Cons:** Size limits

#### Option 4: Shared Memory + Semaphores

```c
// Create shared memory
int shm_fd = shm_open("/chat_shm", O_CREAT | O_RDWR, 0666);
ftruncate(shm_fd, sizeof(chat_buffer_t));
chat_buffer_t *buf = mmap(0, sizeof(chat_buffer_t), 
                          PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

// Synchronization
sem_t *mutex = sem_open("/chat_mutex", O_CREAT, 0644, 1);
sem_wait(mutex);
// Critical section
sem_post(mutex);
```

**Pros:** Fast, efficient  
**Cons:** Complex synchronization

---

### Part 4: Synchronization (10 points)

Handle concurrent access:

```c
// Protect shared resources
sem_t *client_list_mutex;
sem_t *message_queue_mutex;

// Thread-safe operations
void add_client_safe(client_t *client);
void remove_client_safe(int client_id);
void broadcast_message_safe(message_t *msg);
```

**Requirements:**
- No race conditions
- No deadlocks
- Fair access to resources
- Handle signal interruptions (EINTR)

---

## Message Protocol

Define message structure:

```c
#define MSG_MAX_SIZE 256
#define USERNAME_MAX 32

typedef enum {
    MSG_JOIN,           // Client joining
    MSG_LEAVE,          // Client leaving
    MSG_BROADCAST,      // Message to all
    MSG_PRIVATE,        // Private message
    MSG_LIST_USERS,     // Request user list
    MSG_USER_LIST,      // Response with users
    MSG_ERROR,          // Error message
    MSG_PING,           // Keep-alive
    MSG_PONG            // Keep-alive response
} message_type_t;

typedef struct {
    message_type_t type;
    char from_user[USERNAME_MAX];
    char to_user[USERNAME_MAX];
    char text[MSG_MAX_SIZE];
    time_t timestamp;
    int client_id;
} message_t;
```

---

## 📦 Deliverables

```
homework4/
├── src/
│   ├── chat_server.c           # Server implementation
│   ├── chat_client.c           # Client implementation
│   ├── ipc_sockets.c           # UNIX socket IPC
│   ├── ipc_mq.c                # Message queue IPC
│   ├── message.c               # Message protocol
│   └── utils.c                 # Helper functions
├── include/
│   ├── chat.h
│   ├── message.h
│   └── ipc.h
├── tests/
│   ├── test_server.sh          # Start server
│   ├── test_client.sh          # Start multiple clients
│   ├── test_stress.sh          # Stress test
│   └── test_disconnect.sh      # Test disconnections
├── docs/
│   ├── PROTOCOL.md             # Message protocol spec
│   ├── ARCHITECTURE.md         # System design
│   └── TESTING.md              # Test results
├── Makefile
├── README.md
└── demo.mp4
```

---

## 📊 Grading Rubric

| Component | Points | Criteria |
|-----------|--------|----------|
| **Part 1: Server** | 35 | |
| - Accepts multiple clients | 10 | Concurrent connections |
| - Message routing | 10 | Broadcast & private |
| - Client management | 8 | Add/remove/list |
| - Graceful shutdown | 4 | Cleanup resources |
| - Logging | 3 | Activity logs |
| **Part 2: Client** | 25 | |
| - Connect/disconnect | 8 | Robust connection |
| - Send messages | 8 | All message types |
| - Receive messages | 6 | Non-blocking receive |
| - User interface | 3 | Clean, usable |
| **Part 3: IPC** | 30 | |
| - IPC Method 1 | 15 | Fully functional |
| - IPC Method 2 | 15 | Fully functional |
| **Part 4: Synchronization** | 10 | |
| - No race conditions | 5 | Thread-safe |
| - No deadlocks | 3 | Proper locking |
| - Error handling | 2 | EINTR, timeouts |
| **Code Quality** | 10 | |
| - Modular design | 3 | Separated files |
| - Error handling | 3 | All paths |
| - Memory management | 2 | No leaks |
| - Code style | 2 | Consistent |
| **Documentation** | 10 | |
| - README | 4 | Complete |
| - Protocol spec | 3 | Clear |
| - Demo video | 3 | Shows features |

**Total: 100 points**

**Bonus:**
- File transfer (+10 points)
- Encryption (+10 points)
- Chat history (+5 points)

---

## 🧪 Testing Scenarios

### Test 1: Basic Communication
```bash
# Terminal 1
./chat_server

# Terminal 2
./chat_client
Username: alice
alice> Hello world!

# Terminal 3
./chat_client
Username: bob
bob> Hi alice!
```

### Test 2: Multiple Clients
```bash
# Start server
./chat_server &

# Start 10 clients
for i in {1..10}; do
    echo "user$i" | ./chat_client &
done

# Send messages from all
```

### Test 3: Disconnections
```bash
# Start 3 clients, kill one suddenly
./chat_client &
PID=$!
sleep 2
kill -9 $PID

# Server should detect and cleanup
```

### Test 4: Private Messages
```bash
alice> /msg bob Secret message
bob> /msg alice Got it!
```

---

## 💡 Implementation Tips

### Non-Blocking Receive

```c
// Set socket to non-blocking
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);

// Use select() to check for data
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(server_fd, &readfds);

struct timeval timeout = {1, 0};  // 1 second
if (select(server_fd + 1, &readfds, NULL, NULL, &timeout) > 0) {
    // Data available
    recv(server_fd, buffer, sizeof(buffer), 0);
}
```

### Client List Management

```c
typedef struct client {
    int fd;
    char username[USERNAME_MAX];
    time_t last_seen;
    struct client *next;
} client_t;

client_t *client_list = NULL;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_client(client_t *client) {
    pthread_mutex_lock(&list_mutex);
    client->next = client_list;
    client_list = client;
    pthread_mutex_unlock(&list_mutex);
}
```

---

## 📚 Resources

- `man 7 unix` - UNIX domain sockets
- `man 7 mq_overview` - POSIX message queues
- `man 7 shm_overview` - Shared memory
- `man 7 sem_overview` - Semaphores
- Beej's Guide to Network Programming

---

## ❓ FAQ

**Q: Can I use threads instead of processes?**  
A: Yes! Threads with IPC are acceptable.

**Q: Do I need to handle NAT traversal?**  
A: No, local IPC only (same machine).

**Q: What if a message is larger than buffer?**  
A: Split into chunks or reject with error.

**Q: How do I test with 100 clients?**  
A: Write a test script that spawns clients automatically.

---

**Good luck! Build something awesome!** 💬🚀

