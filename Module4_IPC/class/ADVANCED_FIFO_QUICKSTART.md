# Advanced FIFO Demos - Quick Start Guide

## 🚀 Compilation

```bash
cd /Users/jerome/test_os/Module4_IPC/class
make advanced-fifo
```

---

## 📝 Demo 1: Multiple Writers, One Reader

### Open 4 Terminals

**Terminal 1 - Reader:**
```bash
./fifo_multi_reader
```

**Terminal 2 - Writer 1:**
```bash
./fifo_multi_writer 1
```

**Terminal 3 - Writer 2:**
```bash
./fifo_multi_writer 2
```

**Terminal 4 - Writer 3:**
```bash
./fifo_multi_writer 3
```

### Try It
- Type messages in any writer terminal
- Watch them all appear in the reader!
- All writers can send simultaneously
- Type `quit` in any writer to disconnect it

### Key Points
✓ Multiple processes writing to ONE reader  
✓ Messages are tagged with writer ID  
✓ Writers can connect/disconnect dynamically  
✓ Demonstrates concurrent IPC  

---

## 💬 Demo 2: Bidirectional Communication (Client-Server)

### Open 2 Terminals

**Terminal 1 - Server:**
```bash
./fifo_bidir_server
```
(Wait for "Waiting for client to connect...")

**Terminal 2 - Client:**
```bash
./fifo_bidir_client
```

### Try It
- Type messages in the client terminal
- Server receives, processes (converts to uppercase), and responds
- Client displays the response
- Type `quit` to exit

### Key Points
✓ Full duplex: Client sends requests, server responds  
✓ Uses TWO FIFOs for bidirectional flow  
✓ Request-response pattern  
✓ Server processes data before responding  

---

## 🔍 What's Happening Behind the Scenes?

### Multiple Writers Demo:
```
Writer 1 ---|
Writer 2 ---|--> /tmp/multi_writer_fifo --> Reader
Writer 3 ---|
```
- ONE FIFO shared by all writers
- Kernel handles concurrent writes
- Atomic if messages < 4096 bytes

### Bidirectional Demo:
```
Client --> /tmp/client_to_server --> Server
Client <-- /tmp/server_to_client <-- Server
```
- TWO FIFOs (one per direction)
- Synchronous request-response
- Each process reads from one, writes to other

---

## 🧪 Experiments to Try

### Experiment 1: Dynamic Writers
1. Start reader
2. Start writer 1, send messages
3. Start writer 2, send messages  
4. Kill writer 1 (Ctrl+C or type `quit`)
5. Writer 2 still works!
6. Restart writer 1 - it reconnects

**Demonstrates**: Writers can connect/disconnect independently

### Experiment 2: Message Ordering
1. Have 3 writers open
2. Send messages rapidly from different writers
3. Observe order in reader

**Question**: Are messages in order sent, or interleaved?  
**Answer**: Order depends on kernel scheduling - no guarantee!

### Experiment 3: Server Processing
1. In bidirectional demo, send: `hello world`
2. Server responds: `PROCESSED: HELLO WORLD`
3. Try different messages

**Shows**: Server can process/transform data

### Experiment 4: What if Reader Dies?
1. Multiple writers demo running
2. Kill the reader (Ctrl+C in reader terminal)
3. Try sending from a writer

**Result**: Writers get "Broken pipe" error - no reader!

---

## 🛠️ Cleanup

```bash
# Remove FIFO files
rm -f /tmp/multi_writer_fifo /tmp/client_to_server /tmp/server_to_client

# Or use make clean
make clean
```

---

## 📚 Learn More

See `ADVANCED_FIFO_DEMOS.md` for:
- Detailed explanations
- Teaching points
- Real-world use cases
- Advanced experiments
- Troubleshooting guide

---

## 🎯 Use Cases

### Multiple Writers Pattern
- **Logging systems**: Multiple apps → central logger
- **Monitoring**: Multiple sensors → data collector
- **Event aggregation**: Multiple sources → event processor

### Bidirectional Pattern
- **Request-response services**: Client queries, server responds
- **RPC (Remote Procedure Call)**: Client invokes, server executes
- **Interactive protocols**: Database clients, remote shells

---

## ⚡ Quick Commands

```bash
# Compile all advanced demos
make advanced-fifo

# List active FIFOs
ls -l /tmp/*fifo* /tmp/client_to_server /tmp/server_to_client 2>/dev/null

# Monitor FIFOs in real-time
watch -n 0.5 'ls -l /tmp/*fifo* 2>/dev/null'

# Clean everything
make clean

# Help
make help
```

---

## 🎓 Next Steps

After mastering these patterns:
1. Try the message queue demos (`queue_sender`, `queue_receiver`)
2. Explore semaphores (`simple_semaphore`)
3. Compare FIFO vs Message Queue vs Shared Memory
4. Move to Unix domain sockets for more flexibility

---

## 🐛 Common Issues

**"No such file or directory" when opening FIFO**
→ Start the reader FIRST (it creates the FIFO)

**Writer hangs on open()**
→ FIFO blocks until both ends connected - start reader first

**"Broken pipe" error**
→ Reader died/exited while writer was sending - restart reader

**Messages interleaved/corrupted**
→ Messages too large (> 4096 bytes) - keep them smaller

---

Enjoy exploring advanced FIFO patterns! 🚀

