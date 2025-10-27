# Advanced FIFO Demos - Quick Reference Card

## 🎯 30-Second Setup

```bash
cd /Users/jerome/test_os/Module4_IPC/class
make advanced-fifo
```

---

## 📋 Demo 1: Multiple Writers → One Reader

**Concept**: 3 writers sending to 1 reader simultaneously

```bash
# Terminal 1          # Terminal 2              # Terminal 3              # Terminal 4
./fifo_multi_reader   ./fifo_multi_writer 1    ./fifo_multi_writer 2    ./fifo_multi_writer 3
```

**What to type**: Any message in writer terminals  
**What you'll see**: All messages appear in reader  
**Key learning**: Concurrent IPC, message ordering

---

## 💬 Demo 2: Bidirectional Client ↔ Server

**Concept**: Request-response pattern using 2 FIFOs

```bash
# Terminal 1              # Terminal 2
./fifo_bidir_server       ./fifo_bidir_client
```

**What to type**: Messages in client terminal  
**What you'll see**: Server responds with UPPERCASE version  
**Key learning**: Full-duplex IPC, request-response

---

## 🔥 Quick Commands

| Task | Command |
|------|---------|
| Compile all | `make advanced-fifo` |
| Clean up | `make clean` |
| List FIFOs | `ls -l /tmp/*fifo* /tmp/client_to_server /tmp/server_to_client` |
| Remove FIFOs | `rm -f /tmp/multi_writer_fifo /tmp/client_to_server /tmp/server_to_client` |
| Help | `make help` |

---

## 📖 Documentation Guide

| File | Purpose | When to Use |
|------|---------|-------------|
| `ADVANCED_FIFO_QUICKSTART.md` | Getting started | **First time users** |
| `ADVANCED_FIFO_DEMOS.md` | Full guide | **Instructors/deep dive** |
| `DEMO_EXAMPLE_OUTPUT.md` | Expected output | **Troubleshooting** |
| `ADVANCED_FIFO_README.md` | Complete overview | **Course planning** |

---

## ⚡ One-Liner Tests

### Test Multiple Writers
```bash
# Start reader in background, send from 2 writers
./fifo_multi_reader & sleep 1 && echo "Writer1 msg" | ./fifo_multi_writer 1 & echo "Writer2 msg" | ./fifo_multi_writer 2
```

### Test Bidirectional
```bash
# Start server in background, client sends test message
./fifo_bidir_server & sleep 1 && echo "test message" | ./fifo_bidir_client
```

---

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| "No such file" | Start **reader/server first** |
| Hangs on open | Reader not running - check Terminal 1 |
| "Broken pipe" | Reader died - restart it |
| Messages garbled | Messages too large (keep < 4096 bytes) |

---

## 🎓 Teaching Sequence

1. **Compile** (1 min)
2. **Demo 1** - Multiple writers (15 min)
3. **Demo 2** - Bidirectional (15 min)
4. **Experiments** - Try variations (10 min)
5. **Discussion** - Real-world applications (10 min)

**Total**: ~50 minutes

---

## 🧪 Quick Experiments

### Experiment 1: Message Order
Send messages from 3 writers simultaneously  
→ See non-deterministic ordering

### Experiment 2: Dynamic Connection
Kill one writer (Ctrl+C), restart it  
→ See seamless reconnection

### Experiment 3: Response Time
Server processes requests synchronously  
→ Client waits for each response

---

## 💡 Key Takeaways

### Multiple Writers
✓ Many-to-one communication  
✓ One FIFO shared by all writers  
✓ Non-deterministic ordering  
✓ Use case: Logging, monitoring  

### Bidirectional
✓ One-to-one communication  
✓ Two FIFOs (one per direction)  
✓ Synchronous request-response  
✓ Use case: RPC, services  

---

## 🎯 Success Checklist

- [ ] Compiled all programs without errors
- [ ] Ran multiple writers demo with 3+ writers
- [ ] Saw messages from all writers in reader
- [ ] Ran bidirectional demo
- [ ] Sent requests and received responses
- [ ] Tried at least one experiment
- [ ] Understand the difference between the two patterns

---

## 🚀 Next Steps

After mastering these:
1. Try message queues (`queue_sender`, `queue_receiver`)
2. Explore semaphores (`simple_semaphore`)
3. Compare performance of different IPC methods
4. Build your own application using these patterns

---

## 📞 Quick Help

**Compilation error?**
→ Check gcc is installed: `gcc --version`

**Can't find programs?**
→ Make sure you're in the right directory:
```bash
pwd
# Should show: /Users/jerome/test_os/Module4_IPC/class
```

**Programs won't run?**
→ Make them executable:
```bash
chmod +x fifo_multi_reader fifo_multi_writer fifo_bidir_server fifo_bidir_client
```

---

**Print this card for quick reference during demos!** 🖨️

