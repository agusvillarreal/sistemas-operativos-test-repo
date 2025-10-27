# System V IPC Demonstration Guide

## Overview
This guide covers two important System V IPC mechanisms:
1. **Semaphores** - Synchronization primitives for coordinating processes
2. **Message Queues** - Structured message passing between processes

---

## Part 1: SEMAPHORES

### What is a Semaphore?

A **semaphore** is a counter used for synchronizing processes. Think of it like a ticket system:
- **Binary semaphore (mutex)**: Value is 0 or 1 - acts as a lock
- **Counting semaphore**: Can have any value - limits resources

### Key Operations:
- **P (wait/down)**: Decrement counter. If 0, BLOCK until available
- **V (signal/up)**: Increment counter. Wake up waiting process

### Your Programs:

1. **`semaphore.c`** - Basic example (runs quickly)
2. **`semaphore_demo.c`** - Enhanced with sleep to show blocking

### Compilation:
```bash
gcc -o semaphore semaphore.c
gcc -o semaphore_demo semaphore_demo.c
```

### Demo 1: Race Condition (Without Semaphore)

Show students what happens WITHOUT synchronization:
```bash
# Open 3 terminals and run simultaneously:
Terminal 1: echo "Process A working..." && sleep 3 && echo "Process A done"
Terminal 2: echo "Process B working..." && sleep 3 && echo "Process B done"
Terminal 3: echo "Process C working..." && sleep 3 && echo "Process C done"
```
**Point out**: All run at the same time! What if they're modifying the same file?

### Demo 2: Semaphore Protection

Now show WITH semaphore protection:

**Terminal 1:**
```bash
./semaphore_demo 1
```

**Terminal 2 (quickly!):**
```bash
./semaphore_demo 2
```

**Terminal 3 (quickly!):**
```bash
./semaphore_demo 3
```

**What Students Will See:**
```
[Process 1] Trying to enter critical section...
[Process 1] ✓ ENTERED critical section (lock acquired)
[Process 1] Working in critical section.....

[Process 2] Trying to enter critical section...
   ← Process 2 is BLOCKED here, waiting!

[Process 1] ✓ LEAVING critical section (releasing lock)
[Process 1] Done!
[Process 2] ✓ ENTERED critical section (lock acquired)
   ← Now Process 2 can enter!
```

### Teaching Points:

1. **Mutual Exclusion**: Only ONE process in critical section at a time
2. **Blocking Behavior**: Other processes WAIT automatically
3. **Fairness**: Processes enter in order (usually)
4. **No Busy-Waiting**: Blocked processes don't consume CPU

### Check IPC Resources:
```bash
# View active semaphores
ipcs -s

# Output shows:
# Semaphore Arrays:
# T     ID     KEY        ...
# s  12345  0x53000016   ...
```

---

## Part 2: MESSAGE QUEUES

### What is a Message Queue?

A **message queue** is like a mailbox system:
- Processes can SEND messages (put in mailbox)
- Processes can RECEIVE messages (take from mailbox)
- Messages can have TYPES (for filtering)
- Messages persist until received (unlike pipes)

### Advantages Over Pipes/FIFOs:
- ✓ Messages are structured (not just byte streams)
- ✓ Messages persist (stored in kernel)
- ✓ Can select messages by type
- ✓ Non-sequential reading (can skip messages)

### Your Programs:

1. **`queue.c`** - Original sender (single message)
2. **`queue_sender.c`** - Interactive sender
3. **`queue_receiver.c`** - Message receiver

### Compilation:
```bash
gcc -o queue queue.c
gcc -o queue_sender queue_sender.c
gcc -o queue_receiver queue_receiver.c
```

### Demo 1: Basic Send/Receive

**Terminal 1 (Receiver - Start First!):**
```bash
./queue_receiver
```
Output: `Waiting for messages (Ctrl+C to exit)...`

**Terminal 2 (Sender):**
```bash
./queue
```
Output in Terminal 1: `📬 Received: Hello from sender`

**Key Point**: Message was sent INSTANTLY and received IMMEDIATELY!

### Demo 2: Interactive Chat

**Terminal 1:**
```bash
./queue_receiver
```

**Terminal 2:**
```bash
./queue_sender
```

Now type messages in Terminal 2:
```
> Hello class!
✓ Message sent to queue
> This is a message queue demo
✓ Message sent to queue
> Messages are stored in kernel memory
✓ Message sent to queue
> quit
```

Each message appears instantly in Terminal 1!

### Demo 3: Persistent Messages

**Show that messages persist:**

**Step 1**: Send messages WITHOUT a receiver
```bash
./queue_sender
> Message 1
> Message 2
> Message 3
> quit
```

**Step 2**: NOW start the receiver
```bash
./queue_receiver
```

**Students will see**: All 3 messages appear immediately! Messages were stored in the queue.

### Teaching Points:

1. **Persistence**: Messages survive until read (unlike pipes)
2. **Kernel Storage**: Messages stored in kernel, not user space
3. **Structured Data**: Can send typed messages (struct)
4. **Blocking**: Receiver blocks if no messages available
5. **Multiple Processes**: Many senders, many receivers possible

### Check Message Queues:
```bash
# View active message queues
ipcs -q

# See queue statistics
ipcs -q -i <queue_id>
```

---

## Part 3: COMPARISON TABLE

| Feature | Pipe/FIFO | Message Queue | Semaphore |
|---------|-----------|---------------|-----------|
| Purpose | Data transfer | Data transfer | Synchronization |
| Data format | Byte stream | Structured messages | Counter only |
| Persistence | No | Yes (kernel) | Yes (kernel) |
| Selection | Sequential | By type | N/A |
| Blocking | Yes | Yes | Yes |
| Use case | Simple IPC | Complex IPC | Process coordination |

---

## Part 4: CLEANUP

### Why Cleanup?

System V IPC objects persist even after programs exit! You MUST manually remove them.

### Manual Cleanup:
```bash
# List all IPC resources
ipcs

# Remove specific semaphore
ipcrm -s <semaphore_id>

# Remove specific message queue
ipcrm -q <queue_id>

# Remove ALL (use carefully!)
ipcrm --all=sem
ipcrm --all=msg
```

### Or use the cleanup utility:
```bash
gcc -o cleanup_ipc cleanup_ipc.c
./cleanup_ipc
```

---

## Part 5: DEMO SEQUENCE FOR CLASS

### Recommended Order:

1. **Start with FIFO** (simplest) - 10 min
   - Show basic pipe communication
   - Two terminals, simple demo

2. **Move to Message Queues** - 15 min
   - Show structured messaging
   - Demonstrate persistence
   - Show multiple senders/receivers

3. **Finish with Semaphores** - 15 min
   - Explain race conditions first
   - Show mutual exclusion demo
   - Run 3 processes simultaneously

4. **Comparison & Use Cases** - 10 min
   - When to use each mechanism
   - Real-world examples

---

## Common Student Questions

**Q: Why use System V IPC instead of modern alternatives?**
A: Legacy code, understanding OS fundamentals. Modern: POSIX IPC, shared memory, sockets.

**Q: What happens if I forget to remove IPC resources?**
A: They persist forever (until reboot)! Use `ipcs` and `ipcrm` to clean up.

**Q: Can processes on different machines use these?**
A: No! System V IPC is local only. Use sockets for network communication.

**Q: What's the difference between semaphore and mutex?**
A: Binary semaphore (value 0/1) acts like a mutex. Counting semaphore can be > 1.

**Q: Why does receiver block if no messages?**
A: Default behavior. Can use IPC_NOWAIT flag for non-blocking.

**Q: How much data can a message queue hold?**
A: System limits (see `/proc/sys/kernel/msgmnb` on Linux, ~16KB typical).

---

## Advanced Demos (If Time Permits)

### Producer-Consumer with Semaphores
Multiple producers adding to a queue, consumers removing from it, semaphores controlling access.

### Priority Messages
Message queues support message types - can receive high-priority messages first.

### Bidirectional Communication
Two message queues - one for each direction of communication.

---

## Troubleshooting

### "File exists" error
```bash
# IPC resource already exists from previous run
./cleanup_ipc
```

### "Permission denied"
```bash
# Someone else's IPC resource with same key
# Use different ftok() parameter or clean up
```

### Can't see messages
```bash
# Check if queue exists
ipcs -q

# Verify sender and receiver use same key
# Both should use ftok("/tmp", 'A')
```

---

## Summary

- **FIFOs**: Simple, byte-stream, named pipes
- **Message Queues**: Structured, persistent, typed messages  
- **Semaphores**: Synchronization, mutual exclusion, resource counting

All are System V IPC mechanisms - older but still widely used and great for learning OS concepts!

