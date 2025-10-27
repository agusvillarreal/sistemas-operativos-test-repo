# Advanced FIFO Demos - Expected Output Examples

This document shows what you'll see when running the advanced FIFO demos.

---

## Demo 1: Multiple Writers, One Reader

### Terminal 1 - Reader Output

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
📬 Message #1: [Writer 1]: Hello from Writer 1!
📬 Message #2: [Writer 2]: Writer 2 here!
📬 Message #3: [Writer 1]: This is my second message
📬 Message #4: [Writer 3]: Writer 3 joining the party!
📬 Message #5: [Writer 2]: Multiple writers work simultaneously!
📬 Message #6: [Writer 1]: This is awesome!
📬 Message #7: [Writer 3]: All messages arrive correctly
```

### Terminal 2 - Writer 1 Output

```
===================================
MULTI-WRITER FIFO: Writer 1
===================================
PID: 12345
Connecting to FIFO: /tmp/multi_writer_fifo

Opening FIFO for writing...
✓ Connected to reader!
Enter messages (type 'quit' to exit):
-----------------------------------
[Writer 1]> Hello from Writer 1!
  ✓ Sent (message #1)
[Writer 1]> This is my second message
  ✓ Sent (message #2)
[Writer 1]> This is awesome!
  ✓ Sent (message #3)
[Writer 1]> quit

Writer 1 exiting.
Total messages sent: 3
```

### Terminal 3 - Writer 2 Output

```
===================================
MULTI-WRITER FIFO: Writer 2
===================================
PID: 12346
Connecting to FIFO: /tmp/multi_writer_fifo

Opening FIFO for writing...
✓ Connected to reader!
Enter messages (type 'quit' to exit):
-----------------------------------
[Writer 2]> Writer 2 here!
  ✓ Sent (message #1)
[Writer 2]> Multiple writers work simultaneously!
  ✓ Sent (message #2)
[Writer 2]> quit

Writer 2 exiting.
Total messages sent: 2
```

### Terminal 4 - Writer 3 Output

```
===================================
MULTI-WRITER FIFO: Writer 3
===================================
PID: 12347
Connecting to FIFO: /tmp/multi_writer_fifo

Opening FIFO for writing...
✓ Connected to reader!
Enter messages (type 'quit' to exit):
-----------------------------------
[Writer 3]> Writer 3 joining the party!
  ✓ Sent (message #1)
[Writer 3]> All messages arrive correctly
  ✓ Sent (message #2)
[Writer 3]> quit

Writer 3 exiting.
Total messages sent: 2
```

---

## Demo 2: Bidirectional Communication

### Terminal 1 - Server Output

```
===================================
BIDIRECTIONAL FIFO SERVER
===================================
Creating FIFOs...
  ✓ Created: /tmp/client_to_server
  ✓ Created: /tmp/server_to_client

Waiting for client to connect...
✓ Client connected!
Server ready to process requests...
(Press Ctrl+C to exit)

-----------------------------------
📨 Request #1 from client: hello server
📤 Response sent: PROCESSED: HELLO SERVER

📨 Request #2 from client: this is a test
📤 Response sent: PROCESSED: THIS IS A TEST

📨 Request #3 from client: bidirectional communication
📤 Response sent: PROCESSED: BIDIRECTIONAL COMMUNICATION

📨 Request #4 from client: convert this to uppercase
📤 Response sent: PROCESSED: CONVERT THIS TO UPPERCASE

[INFO] Client disconnected.
-----------------------------------
Server shutting down.
Total requests processed: 4
```

### Terminal 2 - Client Output

```
===================================
BIDIRECTIONAL FIFO CLIENT
===================================
Connecting to server...
✓ Connected to server!

Send messages to server (it will process and respond)
Type 'quit' to exit
-----------------------------------

[Client]> hello server
  📤 Request sent (#1)
  ⏳ Waiting for server response...
  📨 Server response: PROCESSED: HELLO SERVER

[Client]> this is a test
  📤 Request sent (#2)
  ⏳ Waiting for server response...
  📨 Server response: PROCESSED: THIS IS A TEST

[Client]> bidirectional communication
  📤 Request sent (#3)
  ⏳ Waiting for server response...
  📨 Server response: PROCESSED: BIDIRECTIONAL COMMUNICATION

[Client]> convert this to uppercase
  📤 Request sent (#4)
  ⏳ Waiting for server response...
  📨 Server response: PROCESSED: CONVERT THIS TO UPPERCASE

[Client]> quit

-----------------------------------
Client exiting.
Total requests sent: 4
```

---

## Visual Flow Diagrams

### Multiple Writers, One Reader

```
TIME →

Writer 1:  [Connect]--[Send Msg 1]--------[Send Msg 2]--[Disconnect]
                │           │                   │
Writer 2:       │    [Connect]--[Send Msg 1]----│-------[Send Msg 2]--[Disconnect]
                │           │           │       │            │
Writer 3:       │           │           │   [Connect]--------│--------[Send Msg 1]--[Disconnect]
                │           │           │       │            │            │
Reader:    [Start]--[Recv 1]--[Recv 2]--[Recv 3]--[Recv 4]--[Recv 5]--[Recv 6]--[Continue...]

Messages arrive in unpredictable order!
```

### Bidirectional Communication

```
Client                    Server
  |                          |
  |-- Request: "hello" ----->|
  |                          | (Process: uppercase)
  |<-- Response: "HELLO" ----|
  |                          |
  |-- Request: "test" ------>|
  |                          | (Process: uppercase)
  |<-- Response: "TEST" -----|
  |                          |
  
Synchronous request-response pattern
```

---

## Key Observations

### Multiple Writers Demo

1. **Message Ordering**: Messages don't arrive in a predictable order
   - Depends on kernel scheduling
   - Depends on typing speed
   - No guarantee of fairness

2. **Concurrency**: All writers operate independently
   - No blocking between writers
   - Each writer has its own connection

3. **Identification**: Messages are tagged with sender ID
   - Format: `[Writer X]: message`
   - Essential for multi-writer scenarios

4. **Dynamic Connections**:
   - Writers can join anytime
   - Writers can leave anytime
   - Reader keeps running

### Bidirectional Demo

1. **Synchronous**: Client waits for response
   - Request sent → Client blocks
   - Response arrives → Client unblocks

2. **Processing**: Server transforms data
   - Example: converts to uppercase
   - Real apps: database query, computation, etc.

3. **Two Channels**: Separate FIFOs for each direction
   - client_to_server: client writes, server reads
   - server_to_client: server writes, client reads

4. **Blocking Behavior**:
   - Client blocks on read() waiting for response
   - Server blocks on read() waiting for request

---

## Comparison: What You Learn

| Aspect | Multiple Writers | Bidirectional |
|--------|-----------------|---------------|
| **FIFOs** | 1 | 2 |
| **Communication** | One-way | Two-way |
| **Concurrency** | High (many senders) | Low (1-to-1) |
| **Responses** | None | Yes |
| **Pattern** | Publish-subscribe | Request-response |
| **Real World** | Logging, monitoring | RPC, services |
| **Complexity** | Simple | Moderate |

---

## Try These Experiments

### Experiment 1: Race Condition
Send messages rapidly from all 3 writers simultaneously. 
**Question**: Does order match sending order?
**Answer**: No! Demonstrates non-deterministic behavior.

### Experiment 2: Writer Reconnection
1. Start reader + 2 writers
2. Kill writer 1
3. Restart writer 1
4. Continue sending messages
**Observation**: Writer reconnects seamlessly!

### Experiment 3: Slow Server
Modify server to add `sleep(2)` before responding.
**Observation**: Client waits 2 seconds for each response!

### Experiment 4: Message Size
Send a very long message (> 4096 bytes) from multiple writers.
**Risk**: Messages might interleave and corrupt each other!

---

## Success Criteria

✅ **Multiple Writers Demo Success:**
- All 3 writers can connect
- Messages from all writers appear in reader
- Writers can disconnect/reconnect
- No data loss or corruption

✅ **Bidirectional Demo Success:**
- Client connects to server
- Each request gets a response
- Responses are processed correctly (uppercase)
- Clean shutdown on 'quit'

---

## Next Steps

After running these demos:
1. Read the source code - understand how it works
2. Modify the server processing logic (try other operations)
3. Add more writers (4, 5, 10... how many can it handle?)
4. Try sending very large messages
5. Compare with message queue demos
6. Think about real-world applications

**Question to Ponder**: 
How would you implement a chat system where multiple clients can send messages to each other? Would you use multiple writers, bidirectional, or a combination?

---

End of demo examples. Try it yourself! 🚀

