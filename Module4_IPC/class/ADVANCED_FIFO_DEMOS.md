# Advanced FIFO Demonstrations

## Overview
This guide covers two advanced FIFO (named pipe) patterns:
1. **Multiple Writers, One Reader** - Multiple processes writing to a single reader
2. **Bidirectional Communication** - Full duplex client-server communication

---

## Part 1: MULTIPLE WRITERS, ONE READER

### Concept

FIFOs naturally support **multiple writers**! The kernel handles:
- ✓ Atomic writes (if message < PIPE_BUF, typically 4096 bytes)
- ✓ Fair scheduling between writers
- ✓ No data loss or corruption

### Real-World Use Cases:
- **Logging**: Multiple processes writing to a central log collector
- **Monitoring**: Multiple sensors sending data to one monitor
- **Chat Systems**: Multiple clients sending to a server

### Compilation:
```bash
gcc -o fifo_multi_reader fifo_multi_reader.c
gcc -o fifo_multi_writer fifo_multi_writer.c
```

### Demo: 3 Writers, 1 Reader

**Step 1: Start the Reader (Terminal 1)**
```bash
./fifo_multi_reader
```

You'll see:
```
===================================
MULTI-WRITER FIFO READER
===================================
This reader can receive messages from MULTIPLE writers!

Opening FIFO for reading: /tmp/multi_writer_fifo
Waiting for writers to connect...

✓ FIFO opened successfully!
Ready to receive messages from multiple writers...
(Press Ctrl+C to exit)

-----------------------------------
```

**Step 2: Start Writer 1 (Terminal 2)**
```bash
./fifo_multi_writer 1
```

**Step 3: Start Writer 2 (Terminal 3)**
```bash
./fifo_multi_writer 2
```

**Step 4: Start Writer 3 (Terminal 4)**
```bash
./fifo_multi_writer 3
```

### Interactive Demo

Now send messages from different writers:

**Terminal 2 (Writer 1):**
```
[Writer 1]> Hello from Writer 1!
  ✓ Sent (message #1)
[Writer 1]> I'm sending my second message
  ✓ Sent (message #2)
```

**Terminal 3 (Writer 2):**
```
[Writer 2]> Writer 2 here!
  ✓ Sent (message #1)
[Writer 2]> Concurrent writing works!
  ✓ Sent (message #2)
```

**Terminal 4 (Writer 3):**
```
[Writer 3]> And Writer 3 joining in!
  ✓ Sent (message #1)
```

**Terminal 1 (Reader) shows ALL messages:**
```
📬 Message #1: [Writer 1]: Hello from Writer 1!
📬 Message #2: [Writer 2]: Writer 2 here!
📬 Message #3: [Writer 1]: I'm sending my second message
📬 Message #4: [Writer 2]: Concurrent writing works!
📬 Message #5: [Writer 3]: And Writer 3 joining in!
```

### Teaching Points

1. **Order is NOT Guaranteed**: Messages appear in the order they reach the kernel
2. **No Blocking Between Writers**: Writers don't block each other (until buffer full)
3. **Atomic Writes**: Small messages (< PIPE_BUF) won't be interleaved
4. **Dynamic Connections**: Writers can connect/disconnect anytime
5. **Message Identification**: Each writer labels its messages (best practice)

### Experiment: Close and Restart Writers

1. Type `quit` in Writer 1 (Terminal 2) to exit
2. Writer 1 disconnects, but others keep working!
3. Restart Writer 1 - it reconnects seamlessly
4. The reader continues receiving from all writers

---

## Part 2: BIDIRECTIONAL COMMUNICATION

### Concept

Create **two FIFOs** for full-duplex communication:
- `client_to_server` - Client writes, Server reads
- `server_to_client` - Server writes, Client reads

### Architecture:
```
    CLIENT                      SERVER
      |                            |
      |--- client_to_server ---->  | (read)
      |                            |
      | <-- server_to_client ----- | (write)
      |                            |
```

### Real-World Use Cases:
- **Request-Response**: Client sends query, server responds
- **Remote Procedure Call (RPC)**: Client invokes, server executes
- **Interactive Services**: Database clients, remote shells

### Compilation:
```bash
gcc -o fifo_bidir_server fifo_bidir_server.c
gcc -o fifo_bidir_client fifo_bidir_client.c
```

### Demo: Client-Server Communication

**Step 1: Start the Server (Terminal 1)**
```bash
./fifo_bidir_server
```

You'll see:
```
===================================
BIDIRECTIONAL FIFO SERVER
===================================
Creating FIFOs...
  ✓ Created: /tmp/client_to_server
  ✓ Created: /tmp/server_to_client

Waiting for client to connect...
```

**Step 2: Start the Client (Terminal 2)**
```bash
./fifo_bidir_client
```

Client shows:
```
===================================
BIDIRECTIONAL FIFO CLIENT
===================================
Connecting to server...
✓ Connected to server!

Send messages to server (it will process and respond)
Type 'quit' to exit
-----------------------------------
```

Server shows:
```
✓ Client connected!
Server ready to process requests...
(Press Ctrl+C to exit)

-----------------------------------
```

### Interactive Demo

**Terminal 2 (Client):**
```
[Client]> hello server
  📤 Request sent (#1)
  ⏳ Waiting for server response...
  📨 Server response: PROCESSED: HELLO SERVER

[Client]> this is a test message
  📤 Request sent (#2)
  ⏳ Waiting for server response...
  📨 Server response: PROCESSED: THIS IS A TEST MESSAGE

[Client]> bidirectional communication works!
  📤 Request sent (#3)
  ⏳ Waiting for server response...
  📨 Server response: PROCESSED: BIDIRECTIONAL COMMUNICATION WORKS!
```

**Terminal 1 (Server):**
```
📨 Request #1 from client: hello server
📤 Response sent: PROCESSED: HELLO SERVER

📨 Request #2 from client: this is a test message
📤 Response sent: PROCESSED: THIS IS A TEST MESSAGE

📨 Request #3 from client: bidirectional communication works!
📤 Response sent: PROCESSED: BIDIRECTIONAL COMMUNICATION WORKS!
```

### Teaching Points

1. **Request-Response Pattern**: Client waits for response (blocking read)
2. **Two Separate Channels**: Each direction has its own FIFO
3. **Role Symmetry**: Each process is both reader AND writer
4. **Processing Logic**: Server transforms data (uppercase conversion example)
5. **Synchronous Communication**: Client blocks until response arrives

### How It Works

**Server Side:**
```c
fd_read = open(CLIENT_TO_SERVER, O_RDONLY);   // Read requests
fd_write = open(SERVER_TO_CLIENT, O_WRONLY);  // Write responses
```

**Client Side:**
```c
fd_write = open(CLIENT_TO_SERVER, O_WRONLY);  // Write requests
fd_read = open(SERVER_TO_CLIENT, O_RDONLY);   // Read responses
```

### Experiment: Server Processing Time

Modify the server to add delays:

```c
printf("📨 Request #%d from client: %s\n", request_count, buffer);
sleep(3);  // Simulate processing time
snprintf(response, sizeof(response), "PROCESSED: ");
```

Now the client will wait 3 seconds for each response!

---

## Part 3: COMPARISON

### Multiple Writers vs Bidirectional

| Feature | Multiple Writers | Bidirectional |
|---------|-----------------|---------------|
| FIFOs needed | 1 | 2 |
| Communication | One-way | Two-way |
| Senders | Multiple | Typically 1-1 |
| Response | No | Yes |
| Complexity | Simple | Moderate |
| Use case | Logging, broadcast | RPC, services |

---

## Part 4: ADVANCED EXPERIMENTS

### Experiment 1: Message Size Limits

**Test atomic write guarantee:**

In writer, send a LARGE message (> 4096 bytes):
```c
char large_msg[5000];
memset(large_msg, 'A', sizeof(large_msg) - 1);
write(fd, large_msg, sizeof(large_msg));
```

With multiple writers sending large messages simultaneously, messages may **interleave**!

**Key Point**: Keep messages < PIPE_BUF (4096 bytes) for atomicity

### Experiment 2: Buffer Overflow

**Fill the FIFO buffer:**

1. Start reader but DON'T read (comment out the read loop)
2. Writer keeps writing
3. Eventually writer BLOCKS (buffer full)
4. Resume reading - writer unblocks!

**Demonstrates**: FIFOs have finite buffer (typically 64KB on Linux)

### Experiment 3: Non-Blocking I/O

**Modify writer to use O_NONBLOCK:**
```c
fd = open(fifo_path, O_WRONLY | O_NONBLOCK);
```

Now if buffer is full, write() returns -1 with errno = EAGAIN instead of blocking!

### Experiment 4: Multiple Servers

**Can we have multiple readers?**

Try starting two `fifo_multi_reader` processes...

**Result**: Only ONE can read a message! Messages are consumed (not broadcast).

For broadcast, need different mechanisms (message queues, shared memory, sockets).

---

## Part 5: DEBUGGING AND MONITORING

### Check FIFO Status

```bash
# List FIFOs
ls -l /tmp/*fifo* /tmp/client_to_server /tmp/server_to_client

# Output shows 'p' for pipe:
# prw-rw-rw- 1 user user 0 Oct 27 09:30 /tmp/multi_writer_fifo
```

### Monitor in Real-Time

**Terminal 1:**
```bash
watch -n 0.5 'ls -l /tmp/*fifo* 2>/dev/null'
```

**Terminal 2:**
```bash
# Run demos and watch the FIFOs appear/disappear
```

### Check for Deadlocks

**Common mistake:**
```c
// WRONG: Can deadlock!
fd_write = open(SERVER_TO_CLIENT, O_WRONLY);  // Blocks!
fd_read = open(CLIENT_TO_SERVER, O_RDONLY);   // Never reached
```

**Solution**: Open in correct order (reader first, or use O_NONBLOCK)

---

## Part 6: REAL-WORLD PATTERNS

### Pattern 1: Log Aggregator

```
Application 1 ---|
Application 2 ---|--> FIFO --> Log Collector --> File/Database
Application 3 ---|
```

### Pattern 2: Request-Response Service

```
Client <---> FIFO 1 <---> Server
             FIFO 2
```

### Pattern 3: Pipeline

```
Generator --> FIFO 1 --> Processor --> FIFO 2 --> Consumer
```

Each stage is a separate process!

---

## Summary

### Multiple Writers, One Reader
- ✓ Simple concurrent communication
- ✓ Single FIFO, multiple senders
- ✓ Great for logging and monitoring
- ✗ No responses to senders
- ✗ Messages consumed (not broadcast)

### Bidirectional Communication
- ✓ Request-response pattern
- ✓ Full-duplex communication
- ✓ Server can process requests
- ✗ Requires two FIFOs
- ✗ Typically 1-to-1 (not many-to-many)

### When to Use Each
- **Multiple Writers**: Logging, event collection, monitoring
- **Bidirectional**: Services, RPC, interactive protocols

### When NOT to Use FIFOs
- ❌ Network communication (use sockets)
- ❌ Broadcast to multiple readers (use message queues)
- ❌ Shared data structures (use shared memory)
- ❌ Very high performance (use shared memory + semaphores)

---

## Cleanup

```bash
# Remove leftover FIFOs
rm -f /tmp/multi_writer_fifo /tmp/client_to_server /tmp/server_to_client

# Or let the programs clean up automatically (they call unlink())
```

---

## Class Demo Sequence

### Demo 1: Multiple Writers (15 min)
1. Explain concept (2 min)
2. Start reader (1 min)
3. Start 3 writers in different terminals (2 min)
4. Have students send messages interactively (5 min)
5. Show message ordering, discuss atomicity (5 min)

### Demo 2: Bidirectional (15 min)
1. Explain two-FIFO pattern (2 min)
2. Start server (1 min)
3. Start client (1 min)
4. Send requests, observe responses (6 min)
5. Discuss request-response pattern, real-world uses (5 min)

### Demo 3: Experiments (10 min)
1. Kill and restart writers - show resilience
2. Try large messages - demonstrate interleaving
3. Compare with previous pipe/FIFO demos

---

## Troubleshooting

### "Broken pipe" Error
- Writer tried to write but no reader exists
- Start reader first!

### Hangs on `open()`
- Opening FIFO blocks until both ends connected
- Start reader before writer, or use O_NONBLOCK

### Messages Lost
- Check that messages < PIPE_BUF for atomicity
- Verify writer flushed/closed properly

### Can't Remove FIFO
- Processes still have it open
- Kill all processes, then `rm -f /tmp/...`

---

## Next Steps

After mastering these FIFO patterns, explore:
1. **POSIX Message Queues** - Better for structured messages
2. **Shared Memory** - Fastest IPC for large data
3. **Unix Domain Sockets** - More flexible than FIFOs
4. **TCP Sockets** - For network communication

These FIFO patterns are foundational for understanding all IPC mechanisms!

