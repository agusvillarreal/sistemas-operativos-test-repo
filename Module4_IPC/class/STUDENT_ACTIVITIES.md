# Student Activities Guide - IPC Demos

## Overview

This guide provides hands-on activities for all Inter-Process Communication (IPC) demos in Module 4. Activities are organized by mechanism and difficulty level.

---

## 📋 Table of Contents

1. [Pipes Activities](#pipes-activities)
2. [Basic FIFO Activities](#basic-fifo-activities)
3. [Advanced FIFO Activities](#advanced-fifo-activities)
4. [Message Queue Activities](#message-queue-activities)
5. [Semaphore Activities](#semaphore-activities)
6. [Cross-Mechanism Projects](#cross-mechanism-projects)
7. [Assessment Rubric](#assessment-rubric)

---

## 🔧 Pipes Activities

**Files**: `pipes.c`

### Activity 1.1: Bidirectional Pipe Communication (⭐ Beginner, 20 min)

**Learning Objective**: Understand pipe directionality

**Task**: Modify `pipes.c` to create TWO pipes so parent and child can communicate bidirectionally.

**Steps**:
1. Create two pipes: `pipe1` and `pipe2`
2. Parent writes to `pipe1`, reads from `pipe2`
3. Child reads from `pipe1`, writes to `pipe2`
4. Send messages in both directions

**Expected Output**:
```
Parent: Sending "Hello Child"
Child: Received "Hello Child"
Child: Sending "Hello Parent"
Parent: Received "Hello Parent"
```

**Hints**:
- Declare: `int pipe1[2], pipe2[2];`
- Close unused ends in each process
- Use proper read/write ordering to avoid deadlock

---

### Activity 1.2: Pipe Calculator (⭐⭐ Intermediate, 30 min)

**Learning Objective**: Use pipes for structured communication

**Task**: Create a calculator where parent sends math expressions and child computes results.

**Requirements**:
1. Parent sends: "5 + 3" via pipe
2. Child parses, computes (8)
3. Child sends result back via second pipe
4. Parent displays: "Result: 8"

**Challenges**:
- Parse the expression string
- Handle multiple operations (+, -, *, /)
- Validate input

**Extension**: Support multiple operations: "5 + 3 * 2"

---

### Activity 1.3: Pipeline Simulation (⭐⭐⭐ Advanced, 45 min)

**Learning Objective**: Chain multiple processes with pipes

**Task**: Simulate Unix pipeline: `ps aux | grep firefox | wc -l`

**Requirements**:
1. Process 1: Generate data (list of processes)
2. Process 2: Filter data (find matching lines)
3. Process 3: Count lines
4. Use pipes to connect them

**Code Structure**:
```c
fork() three times
pipe1: Process1 -> Process2
pipe2: Process2 -> Process3
Each process reads, transforms, writes
```

**Deliverable**: Working 3-process pipeline with pipes

---

## 📂 Basic FIFO Activities

**Files**: `fifo.c`, `fifo_reader.c`, `fifo_writer.c`

### Activity 2.1: Chat Application (⭐ Beginner, 25 min)

**Learning Objective**: Build simple IPC application

**Task**: Enhance FIFO writer/reader into a basic chat system.

**Requirements**:
1. Modify `fifo_writer.c` to prompt for username
2. Prepend username to each message: `[Alice]: Hello`
3. Modify `fifo_reader.c` to display formatted messages
4. Add timestamps

**Expected Output**:
```
[09:30:15] [Alice]: Hello everyone!
[09:30:22] [Bob]: Hi Alice!
```

**Extension**: Add colors using ANSI codes

---

### Activity 2.2: File Transfer via FIFO (⭐⭐ Intermediate, 40 min)

**Learning Objective**: Transfer binary data through FIFOs

**Task**: Send a file from writer to reader via FIFO.

**Requirements**:
1. Writer reads file in chunks
2. Sends each chunk via FIFO
3. Reader receives and writes to new file
4. Verify file integrity (compare checksums)

**Challenges**:
- Handle binary data (not just text)
- Deal with EOF
- Ensure complete transfer
- Verify data integrity

**Test**: Transfer an image file and verify it opens correctly

---

### Activity 2.3: FIFO-Based Command Server (⭐⭐⭐ Advanced, 60 min)

**Learning Objective**: Build a command execution service

**Task**: Create a server that receives commands via FIFO and executes them.

**Requirements**:
1. Server reads commands from FIFO
2. Executes commands (e.g., "ls", "date", "whoami")
3. Captures output
4. Sends output back via second FIFO
5. Security: Whitelist allowed commands only!

**Security Considerations**:
- Never execute arbitrary commands
- Validate input
- Limit permissions

---

## 🚀 Advanced FIFO Activities

**Files**: `fifo_multi_reader.c`, `fifo_multi_writer.c`, `fifo_bidir_server.c`, `fifo_bidir_client.c`

### Activity 3.1: Distributed Logging System (⭐⭐ Intermediate, 45 min)

**Learning Objective**: Apply multiple-writers pattern

**Task**: Build a logging system where multiple applications log to one collector.

**Requirements**:
1. Use `fifo_multi_reader.c` as base
2. Add log levels (INFO, WARN, ERROR)
3. Add timestamps
4. Color-code by severity
5. Optional: Write logs to file

**Format**:
```
[2025-10-27 09:30:15] [INFO] [App1]: Server started
[2025-10-27 09:30:16] [ERROR] [App2]: Connection failed
```

**Extension**: 
- Filter logs by level
- Rotate log files
- Add log statistics (count by level)

---

### Activity 3.2: Enhanced RPC Service (⭐⭐⭐ Advanced, 60 min)

**Learning Objective**: Build request-response service

**Task**: Extend `fifo_bidir_server.c` to support multiple operations.

**Requirements**:
1. Support operations: UPPERCASE, lowercase, reverse, length, count_vowels
2. Client sends: "REVERSE:hello world"
3. Server responds: "dlrow olleh"
4. Handle errors gracefully

**Protocol Design**:
```
Request:  OPERATION:data
Response: RESULT:data  or  ERROR:message
```

**Operations to Implement**:
- `UPPERCASE:text` → `RESULT:TEXT`
- `REVERSE:text` → `RESULT:txet`
- `LENGTH:text` → `RESULT:4`
- `COUNT_VOWELS:hello` → `RESULT:2`

**Extension**: Add file operations (read, write, delete)

---

### Activity 3.3: Load Balanced Service (⭐⭐⭐⭐ Expert, 90 min)

**Learning Objective**: Implement advanced server architecture

**Task**: Create a server pool where multiple servers handle requests.

**Architecture**:
```
Multiple Clients --> Dispatcher --> Server Pool (3 servers)
                                       |
                                    Results
```

**Requirements**:
1. Dispatcher receives requests from multiple writers
2. Distributes requests to servers (round-robin or least-busy)
3. Each server processes independently
4. Results sent back via dedicated FIFOs

**Challenges**:
- Track which server is handling which request
- Handle server failures
- Load balancing algorithm

**Deliverable**: System with 3+ clients, 1 dispatcher, 3+ servers

---

## 📬 Message Queue Activities

**Files**: `queue.c`, `queue_sender.c`, `queue_receiver.c`

### Activity 4.1: Priority Message System (⭐⭐ Intermediate, 35 min)

**Learning Objective**: Use message types for priorities

**Task**: Modify queue programs to support priority messages.

**Requirements**:
1. Message types: 1=LOW, 2=NORMAL, 3=HIGH, 4=CRITICAL
2. Sender allows choosing priority
3. Receiver processes HIGH priority first
4. Use `msgrcv()` with type parameter

**Example**:
```c
// Receive HIGH priority messages first
msgrcv(msgid, &msg, size, 3, 0);  // type=3 (HIGH)
```

**Test**:
1. Send: LOW, HIGH, NORMAL, CRITICAL
2. Verify: CRITICAL, HIGH, NORMAL, LOW order

---

### Activity 4.2: Multi-Channel Communication (⭐⭐ Intermediate, 40 min)

**Learning Objective**: Use message types as channels

**Task**: Create a system with separate channels (like Slack channels).

**Requirements**:
1. Channels: GENERAL (type=1), TECH (type=2), RANDOM (type=3)
2. Sender specifies channel
3. Receiver can subscribe to specific channels
4. Multiple receivers can listen to different channels

**Format**:
```
[TECH] Alice: Check out this code
[GENERAL] Bob: Meeting at 2pm
[RANDOM] Carol: Anyone want coffee?
```

**Extension**: Receiver subscribes to multiple channels

---

### Activity 4.3: Job Queue System (⭐⭐⭐ Advanced, 75 min)

**Learning Objective**: Build producer-consumer with message queues

**Task**: Create a job processing system with multiple workers.

**Architecture**:
```
Job Producers --> Message Queue --> Worker Pool
                                       |
                                   Job Results
```

**Requirements**:
1. Producers submit jobs (type=1): "COMPUTE:factorial:10"
2. Workers pull jobs from queue
3. Workers process jobs
4. Workers send results (type=2)
5. Support multiple job types: COMPUTE, TRANSFORM, VALIDATE

**Job Types**:
- `COMPUTE:factorial:N` → Calculate factorial
- `COMPUTE:fibonacci:N` → Calculate Fibonacci
- `TRANSFORM:uppercase:text` → Convert to uppercase
- `TRANSFORM:reverse:text` → Reverse string

**Deliverable**: 3 producers, 5 workers, 1 result collector

---

## 🔒 Semaphore Activities

**Files**: `semaphore.c`, `simple_semaphore.c`, `semaphore_demo.c`

### Activity 5.1: Bank Account Synchronization (⭐⭐ Intermediate, 40 min)

**Learning Objective**: Prevent race conditions

**Task**: Simulate bank account with concurrent deposits/withdrawals.

**Scenario**:
- Shared variable: `int account_balance = 1000`
- 10 processes each try to deposit $100
- 10 processes each try to withdraw $50
- Expected final balance: 1000 + 1000 - 500 = 1500

**Without Semaphore**: Balance will be WRONG due to race conditions
**With Semaphore**: Balance will be correct (1500)

**Implementation**:
1. Use shared memory for `account_balance`
2. Use semaphore to protect access
3. Each process: acquire lock → modify balance → release lock
4. Compare results with/without semaphore

**Deliverable**: Demonstration showing race condition and fix

---

### Activity 5.2: Reader-Writer Problem (⭐⭐⭐ Advanced, 60 min)

**Learning Objective**: Implement classic synchronization problem

**Task**: Multiple readers can read simultaneously, but writers need exclusive access.

**Rules**:
- Multiple readers can read at the same time
- Only ONE writer at a time
- No readers while writer is writing
- No writer while readers are reading

**Semaphores Needed**:
```c
sem_mutex       // Protects reader_count
sem_write       // Exclusive access for writing
int reader_count = 0;
```

**Algorithm**:
```
Reader:
  wait(mutex)
  reader_count++
  if (reader_count == 1) wait(write)  // First reader blocks writers
  signal(mutex)
  
  READ DATA
  
  wait(mutex)
  reader_count--
  if (reader_count == 0) signal(write)  // Last reader unblocks writers
  signal(mutex)

Writer:
  wait(write)
  WRITE DATA
  signal(write)
```

**Test**: 5 readers, 2 writers, verify correctness

---

### Activity 5.3: Dining Philosophers (⭐⭐⭐⭐ Expert, 90 min)

**Learning Objective**: Solve complex synchronization problem

**Task**: Implement the famous dining philosophers problem.

**Problem**:
- 5 philosophers sit at round table
- 5 chopsticks (one between each pair)
- Each philosopher: THINK → pick up 2 chopsticks → EAT → put down chopsticks
- Goal: No deadlock, no starvation

**Naive Solution (DEADLOCKS)**:
```c
pick_up(left_chopstick);
pick_up(right_chopstick);
eat();
put_down(right_chopstick);
put_down(left_chopstick);
```
If all pick up left simultaneously → DEADLOCK!

**Solutions to Implement**:
1. **Resource Ordering**: Odd philosophers pick left first, even pick right first
2. **Limit Diners**: Allow only 4 philosophers to attempt eating at once
3. **Waiter**: Central coordinator grants permission

**Deliverable**: 
- Working implementation (no deadlock)
- Demonstration with output showing each philosopher's actions
- Analysis of solution chosen

---

## 🎯 Cross-Mechanism Projects

**These projects combine multiple IPC mechanisms**

### Project 1: Distributed Chat System (⭐⭐⭐ Advanced, 3-4 hours)

**Combines**: FIFOs + Message Queues + Semaphores

**Architecture**:
```
Multiple Clients <--FIFOs--> Server <--MsgQueue--> Storage Process
                                 |
                             Semaphore protects shared state
```

**Features**:
1. Multiple clients send messages via FIFOs
2. Server broadcasts to all clients
3. Message queue stores message history
4. Semaphore protects user list
5. Commands: /users, /history, /quit

**Requirements**:
- Support 10+ concurrent clients
- Store last 100 messages
- Show online/offline status
- Private messages: `/msg Bob Hello`

---

### Project 2: Process Monitor Dashboard (⭐⭐⭐ Advanced, 3-4 hours)

**Combines**: Multiple Writers + Message Queues + Shared Memory

**Architecture**:
```
Monitor Processes --> FIFO --> Aggregator --> Dashboard
     (CPU, Memory,            (filters,         (displays
      Disk, Network)           computes)         metrics)
```

**Features**:
1. Multiple monitor processes collect metrics
2. Send to aggregator via multi-writer FIFO
3. Aggregator computes statistics
4. Stores in message queue
5. Dashboard displays real-time

**Metrics**:
- CPU usage (%)
- Memory usage (MB)
- Disk I/O (KB/s)
- Network traffic (packets/s)

**Deliverable**: Real-time updating dashboard

---

### Project 3: Job Scheduling System (⭐⭐⭐⭐ Expert, 4-6 hours)

**Combines**: All IPC mechanisms

**Architecture**:
```
Job Submitters --> Queue --> Scheduler --> Workers
                                |
                           Semaphores (resource limits)
                                |
                           Shared Memory (job status)
                                |
                           FIFOs (results)
```

**Features**:
1. Submit jobs with priorities
2. Scheduler assigns to workers
3. Limited resources (e.g., max 5 concurrent jobs)
4. Job types: CPU-intensive, I/O-intensive, GPU
5. Status tracking: PENDING, RUNNING, COMPLETED, FAILED

**Requirements**:
- Priority scheduling
- Resource limits (semaphores)
- Job dependencies (Job B runs after Job A)
- Load balancing
- Status API

**Deliverable**: Full job scheduling system with CLI

---

### Project 4: Collaborative Text Editor (⭐⭐⭐⭐ Expert, 6-8 hours)

**Combines**: Bidirectional FIFOs + Shared Memory + Semaphores

**Challenge**: Multiple users editing same file simultaneously

**Features**:
1. Each user connects via bidirectional FIFO
2. File content in shared memory
3. Semaphores prevent concurrent edits to same line
4. Operations: INSERT, DELETE, MODIFY, SAVE
5. Real-time updates to all users

**Challenges**:
- Conflict resolution
- Cursor positions
- Undo/redo
- Permissions

**This is a HARD project!**

---

## 📊 Assessment Rubric

### Code Quality (30%)
- ✅ Compiles without warnings
- ✅ Proper error handling
- ✅ Memory management (no leaks)
- ✅ Code comments and structure
- ✅ Follows style guidelines

### Functionality (40%)
- ✅ Meets all requirements
- ✅ Handles edge cases
- ✅ Robust error handling
- ✅ Performs as expected
- ✅ Passes test cases

### Understanding (20%)
- ✅ Correct IPC mechanism chosen
- ✅ Proper synchronization
- ✅ Demonstrates understanding of concepts
- ✅ Can explain design decisions
- ✅ Documentation quality

### Creativity (10%)
- ✅ Extensions beyond requirements
- ✅ User experience
- ✅ Performance optimizations
- ✅ Novel features

---

## 💡 Tips for Success

### General Tips
1. **Start Simple**: Get basic version working first
2. **Test Incrementally**: Test after each feature
3. **Handle Errors**: Check return values!
4. **Clean Up**: Remove IPC resources when done
5. **Document**: Comment your code

### Debugging IPC
```bash
# Check active IPC resources
ipcs -a

# Remove stuck resources
ipcrm -q <queue_id>
ipcrm -s <semaphore_id>

# Monitor FIFOs
ls -l /tmp/*fifo*

# Check processes
ps aux | grep your_program
```

### Testing Strategies
1. **Unit Test**: Test each component separately
2. **Integration Test**: Test components together
3. **Stress Test**: Many concurrent processes
4. **Error Test**: Intentionally cause errors
5. **Performance Test**: Measure throughput/latency

---

## 📚 Learning Progression

### Week 1: Basics
- Activities 1.1, 1.2 (Pipes)
- Activities 2.1, 2.2 (Basic FIFOs)
- Run all demos, understand output

### Week 2: Intermediate
- Activities 3.1, 3.2 (Advanced FIFOs)
- Activities 4.1, 4.2 (Message Queues)
- Activities 5.1 (Semaphores)

### Week 3: Advanced
- Activities 1.3, 2.3 (Complex pipes/FIFOs)
- Activities 4.3, 5.2 (Jobs, Reader-Writer)
- Start a cross-mechanism project

### Week 4: Expert
- Activity 5.3 (Dining Philosophers)
- Cross-mechanism projects
- Build your own application

---

## 🎓 Grading Criteria

### Individual Activities
| Grade | Criteria |
|-------|----------|
| A | All requirements + extensions, clean code, thorough testing |
| B | All requirements met, good code quality, basic testing |
| C | Most requirements, some bugs, minimal testing |
| D | Partial implementation, significant issues |
| F | Does not compile or run |

### Projects
| Component | Weight |
|-----------|--------|
| Design Document | 10% |
| Implementation | 50% |
| Testing | 20% |
| Documentation | 10% |
| Presentation | 10% |

---

## 🚀 Getting Started

1. **Choose an activity** based on your skill level
2. **Read the requirements** carefully
3. **Plan your approach** before coding
4. **Start with provided code** as a base
5. **Test frequently** during development
6. **Ask for help** when stuck
7. **Submit with documentation** explaining your design

---

## 📞 Resources

- **Manual Pages**: `man pipe`, `man mkfifo`, `man msgget`, `man semget`
- **Demo Code**: All files in this directory
- **Documentation**: IPC_DEMO_GUIDE.md, ADVANCED_FIFO_DEMOS.md
- **Diagrams**: ARCHITECTURE_DIAGRAMS.md

---

## 🎉 Bonus Challenges

1. **Performance**: Measure throughput (messages/second)
2. **Scalability**: Test with 100+ processes
3. **Monitoring**: Add metrics and logging
4. **GUI**: Create graphical interface
5. **Network**: Extend to work across machines

---

**Good luck with your IPC projects!** 🚀

Remember: The best way to learn IPC is by doing. Start simple, test often, and build incrementally!

