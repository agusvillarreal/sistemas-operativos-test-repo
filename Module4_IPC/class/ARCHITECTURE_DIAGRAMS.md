# Advanced FIFO Demos - Architecture Diagrams

## Visual Guide to Understanding the Demos

---

## 🏗️ Demo 1: Multiple Writers, One Reader

### System Architecture

```
                    Operating System Kernel
   ┌────────────────────────────────────────────────────────────┐
   │                                                            │
   │  ┌──────────────────────────────────────────────────┐    │
   │  │         FIFO Buffer (Kernel Space)               │    │
   │  │    /tmp/multi_writer_fifo                        │    │
   │  │                                                   │    │
   │  │    ┌─────┬─────┬─────┬─────┬─────┐              │    │
   │  │    │ Msg │ Msg │ Msg │ ... │ ... │  (up to 64KB)│    │
   │  │    │  1  │  2  │  3  │     │     │              │    │
   │  │    └─────┴─────┴─────┴─────┴─────┘              │    │
   │  │         ▲   ▲   ▲                  │             │    │
   │  │         │   │   │                  ▼             │    │
   │  │       Write Write Write          Read            │    │
   │  └────────┬───┬───┬──────────────────┬─────────────┘    │
   │           │   │   │                  │                   │
   └───────────┼───┼───┼──────────────────┼───────────────────┘
               │   │   │                  │
               │   │   │                  │
        User Space Processes
               │   │   │                  │
               │   │   │                  │
   ┌───────────┴─┐ │   │      ┌───────────┴──────────┐
   │ fifo_multi_ │ │   │      │  fifo_multi_reader   │
   │ writer "1"  │ │   │      │                      │
   │ PID: 1001   │ │   │      │  while(1) {          │
   │             │ │   │      │    read(fd, buf);    │
   │ write(fd,   │ │   │      │    printf(buf);      │
   │  "[W1]: Hi")│ │   │      │  }                   │
   └─────────────┘ │   │      └──────────────────────┘
                   │   │
   ┌───────────────┴─┐ │
   │ fifo_multi_     │ │
   │ writer "2"      │ │
   │ PID: 1002       │ │
   │                 │ │
   │ write(fd,       │ │
   │  "[W2]: Hello") │ │
   └─────────────────┘ │
                       │
   ┌───────────────────┴─┐
   │ fifo_multi_         │
   │ writer "3"          │
   │ PID: 1003           │
   │                     │
   │ write(fd,           │
   │  "[W3]: Hey")       │
   └─────────────────────┘
```

### Data Flow Sequence

```
Time →

t=0:  Reader starts, creates FIFO, blocks on open()
      └─> mkfifo("/tmp/multi_writer_fifo")
      └─> open(O_RDONLY) → BLOCKS

t=1:  Writer 1 connects
      └─> open(O_WRONLY) → UNBLOCKS reader!
      └─> Reader: read() waiting...

t=2:  Writer 2 connects
      └─> open(O_WRONLY) → Success
      └─> Both writers can write now

t=3:  Writer 1 sends message
      Writer 1: write("[W1]: Hello")
      └─────────────────────┐
                            ▼
      Reader: read() → "[W1]: Hello"
      Reader: printf("📬 Message #1: [W1]: Hello")

t=4:  Writer 2 sends message (simultaneously with Writer 1!)
      Writer 2: write("[W2]: Hi")
      └─────────────────────┐
                            ▼
      Reader: read() → "[W2]: Hi"
      Reader: printf("📬 Message #2: [W2]: Hi")

t=5:  Writer 3 connects and sends
      Writer 3: open() then write("[W3]: Hey")
      └─────────────────────┐
                            ▼
      Reader: read() → "[W3]: Hey"
      Reader: printf("📬 Message #3: [W3]: Hey")
```

### Key Observation: Race Condition

```
Three writers send simultaneously:

Writer 1: write("AAAA")  ─┐
Writer 2: write("BBBB")  ─┼─→ Race! Who gets to write first?
Writer 3: write("CCCC")  ─┘

Possible orderings at reader:
  - AAAA, BBBB, CCCC  ✓
  - BBBB, AAAA, CCCC  ✓
  - CCCC, BBBB, AAAA  ✓
  - Any permutation!  ✓

If messages > PIPE_BUF (4096 bytes):
  - AABBBBCC  ❌ Interleaved!
  - BCABCA    ❌ Corrupted!
```

---

## 🏗️ Demo 2: Bidirectional Communication

### System Architecture

```
                    Operating System Kernel
   ┌────────────────────────────────────────────────────────────┐
   │                                                            │
   │  ┌──────────────────────────────────────────────────┐    │
   │  │    FIFO 1: /tmp/client_to_server                 │    │
   │  │    ┌─────┬─────┬─────┐                           │    │
   │  │    │ Req │ Req │ ... │  Client writes →          │    │
   │  │    │  1  │  2  │     │          Server reads ←   │    │
   │  │    └─────┴─────┴─────┘                           │    │
   │  └──────────┬─────────────────────────┬─────────────┘    │
   │             │                         │                   │
   │  ┌──────────┴─────────────────────────┴──────────────┐   │
   │  │    FIFO 2: /tmp/server_to_client                  │   │
   │  │    ┌─────┬─────┬─────┐                            │   │
   │  │    │ Res │ Res │ ... │  Server writes →           │   │
   │  │    │  1  │  2  │     │          Client reads ←    │   │
   │  │    └─────┴─────┴─────┘                            │   │
   │  └──────────┬─────────────────────────┬──────────────┘   │
   │             │                         │                   │
   └─────────────┼─────────────────────────┼───────────────────┘
               │                         │
        User Space Processes
               │                         │
               │                         │
   ┌───────────┴────────────┐  ┌─────────┴────────────┐
   │  fifo_bidir_client     │  │  fifo_bidir_server   │
   │                        │  │                      │
   │  fd_write = open(      │  │  fd_read = open(     │
   │    client_to_server)   │  │    client_to_server) │
   │                        │  │                      │
   │  fd_read = open(       │  │  fd_write = open(    │
   │    server_to_client)   │  │    server_to_client) │
   │                        │  │                      │
   │  // Send request       │  │  // Receive request  │
   │  write(fd_write, req)──┼──┼─→read(fd_read, buf)  │
   │                        │  │                      │
   │  // Wait for response  │  │  // Process          │
   │  read(fd_read, res)◄───┼──┼──process(buf)        │
   │                        │  │                      │
   │  // Display            │  │  // Send response    │
   │  printf(res)           │  │  write(fd_write,res) │
   └────────────────────────┘  └──────────────────────┘
```

### Request-Response Sequence

```
Client                            Server
  │                                 │
  │ 1. open(client_to_server, W)   │
  ├────────────────────────────────▶│ (unblocks server's open(R))
  │                                 │
  │ 2. open(server_to_client, R)   │
  │◄────────────────────────────────┤ (opens server's open(W))
  │                                 │
  │                                 │
  │  ═══ CONNECTION ESTABLISHED ═══│
  │                                 │
  │                                 │
  │ 3. write("hello")               │
  ├────────────────────────────────▶│ 4. read() → "hello"
  │                                 │    Process: "hello" → "HELLO"
  │                                 │
  │ 5. read() [BLOCKS]              │
  │    ⏳ waiting...                │
  │                                 │ 6. write("HELLO")
  │ 7. read() returns "HELLO"       │◄────────────────────────────────┤
  │◄────────────────────────────────┤
  │    ✓ Got response!              │
  │                                 │
  │ 8. printf("Response: HELLO")    │
  │                                 │
  │                                 │
  │ 9. write("test")                │
  ├────────────────────────────────▶│ 10. read() → "test"
  │                                 │     Process: "test" → "TEST"
  │ 11. read() [BLOCKS]             │
  │     ⏳ waiting...               │ 12. write("TEST")
  │ 13. read() returns "TEST"       │◄────────────────────────────────┤
  │◄────────────────────────────────┤
  │                                 │
  │ 14. type "quit"                 │
  │     close(fd_write)             │
  │     close(fd_read)              │ 15. read() returns 0 (EOF)
  │                                 │     Server detects disconnect
  │                                 │     cleanup and exit
  └─────────────────────────────────┴──────────────────────────────────
```

### State Machine View

```
CLIENT STATE MACHINE:

   ┌─────────┐
   │  START  │
   └────┬────┘
        │ open FIFOs
        ▼
   ┌─────────┐
   │  READY  │◄──────────────┐
   └────┬────┘               │
        │ Get user input     │
        ▼                    │
   ┌─────────┐               │
   │ SENDING │               │
   └────┬────┘               │
        │ write(request)     │
        ▼                    │
   ┌─────────┐               │
   │ WAITING │               │
   └────┬────┘               │
        │ read(response)     │
        ▼                    │
   ┌─────────┐               │
   │RECEIVED │───────────────┘
   └────┬────┘
        │ user types "quit"
        ▼
   ┌─────────┐
   │   END   │
   └─────────┘


SERVER STATE MACHINE:

   ┌─────────┐
   │  START  │
   └────┬────┘
        │ create & open FIFOs
        ▼
   ┌─────────┐
   │ WAITING │◄──────────────┐
   │   FOR   │               │
   │ CLIENT  │               │
   └────┬────┘               │
        │ read(request)      │
        ▼                    │
   ┌─────────┐               │
   │RECEIVED │               │
   └────┬────┘               │
        │ process data       │
        ▼                    │
   ┌─────────┐               │
   │PROCESSING               │
   └────┬────┘               │
        │ compute response   │
        ▼                    │
   ┌─────────┐               │
   │ SENDING │               │
   └────┬────┘               │
        │ write(response)    │
        ▼                    │
   ┌─────────┐               │
   │  SENT   │───────────────┘
   └────┬────┘
        │ client disconnects (read=0)
        ▼
   ┌─────────┐
   │   END   │
   └─────────┘
```

### Memory Layout

```
FIFO Buffers in Kernel Memory:

client_to_server FIFO:
┌──────────────────────────────────────────────────┐
│ [h][e][l][l][o][\0]                    [empty]  │ → read by server
└──────────────────────────────────────────────────┘
  ▲                                          ▲
  │                                          │
  write pointer (client)              read pointer (server)

After read: buffer emptied

server_to_client FIFO:
┌──────────────────────────────────────────────────┐
│ [H][E][L][L][O][\0]                    [empty]  │ → read by client
└──────────────────────────────────────────────────┘
  ▲                                          ▲
  │                                          │
  write pointer (server)              read pointer (client)
```

---

## 📊 Comparison: Side-by-Side

```
MULTIPLE WRITERS                    BIDIRECTIONAL
─────────────────                   ─────────────

One FIFO                            Two FIFOs
    │                                   │  │
    │                                   ▼  ▼
    │                               ┌────┬────┐
    ▼                               │ F1 │ F2 │
┌───────┐                           └─┬──┴──┬─┘
│ FIFO  │                             │     │
└───┬───┘                             │     │
    │                                 │     │
    │                                 │     │
  W │ W │ W                         C │     │ S
  r ▼ r ▼ r ▼                       l │     │ e
  i   i   i                         i │     │ r
  t   t   t                         e │     │ v
  e   e   e                         n │     │ e
  r   r   r                         t │     │ r
                                      │     │
  1   2   3                           ▼     ▼
                                    Bidirectional
Many senders                          1-to-1
One receiver                      
No response                       Request-Response
```

---

## 🔍 Deep Dive: What Happens at System Call Level?

### open() Behavior

```
Writer (O_WRONLY):                Reader (O_RDONLY):

1. open(path, O_WRONLY)          1. open(path, O_RDONLY)
   │                                │
   ▼                                ▼
2. System Call                    2. System Call
   │                                │
   ▼                                ▼
3. Kernel: Is FIFO open           3. Kernel: Is FIFO open
   for reading?                      for writing?
   │                                │
   ├─Yes → Return fd               ├─Yes → Return fd
   │                                │
   └─No → BLOCK                    └─No → BLOCK
           (sleep in kernel)              (sleep in kernel)
           │                                │
           Wait for reader...              Wait for writer...
           │                                │
           Reader arrives!                 Writer arrives!
           │                                │
           Wake up!                        Wake up!
           │                                │
           Return fd                       Return fd
```

### write() Behavior

```
Process calls: write(fd, "Hello", 6)

User Space:
   │
   │ write(fd, buf, 6)
   │
   ▼
───────────────────────────────── System Call Boundary
   │
Kernel Space:
   │
   ▼
1. Check: Is FIFO open for reading?
   ├─ No → Return error (EPIPE)
   │
   ├─ Yes → Continue
   │
   ▼
2. Check: Is buffer full?
   ├─ Yes → BLOCK (sleep)
   │         Wait for reader to read...
   │         Wake up when space available
   │
   ├─ No → Continue
   │
   ▼
3. Copy data from user buffer to kernel FIFO buffer
   memcpy(kernel_buf, user_buf, 6)
   │
   ▼
4. Wake up any sleeping readers
   │
   ▼
5. Return bytes written (6)
   │
   ▼
───────────────────────────────── System Call Boundary
   │
User Space:
   │
   ▼
   write() returns 6
```

---

## 🎯 Teaching Points Visual Summary

### Multiple Writers: Key Insight

```
    Traditional              With FIFO
    ─────────────           ──────────

    Each writer             Kernel handles
    directly                synchronization
    modifies file           automatically!
        │                        │
        ▼                        ▼
    ┌─────────┐           ┌──────────┐
    │  FILE   │           │ FIFO BUF │
    │ (race!) │           │(protected)│
    └─────────┘           └──────────┘
        ▲                        ▲
        │                        │
    Corruption!             Safe!
```

### Bidirectional: Key Insight

```
    One FIFO (doesn't work)      Two FIFOs (works!)
    ───────────────────          ──────────────────

         ┌────┐                      ┌────┐
    Req  │    │  Res            Req  │ F1 │
    ────▶│ ?? │◄────            ────▶│    │
         │    │                      └────┘
         └────┘                      ┌────┐
                                 Res │ F2 │
    Both reading/writing        ◄────│    │
    same FIFO = confusion!           └────┘

                                Dedicated channels!
```

---

This architecture guide provides visual understanding of how the advanced FIFO demos work at the system level! 🎓

