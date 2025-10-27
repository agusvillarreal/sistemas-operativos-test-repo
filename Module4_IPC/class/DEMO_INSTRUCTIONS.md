# FIFO (Named Pipe) Class Demonstration

## What is a FIFO?
A FIFO (First In First Out), also called a **named pipe**, is a special type of file used for inter-process communication (IPC). Unlike anonymous pipes, FIFOs:
- Have a name in the filesystem
- Can be used by unrelated processes
- Follow FIFO ordering (first data in is first data out)

## Compilation

```bash
# Compile all programs
gcc -o fifo fifo.c
gcc -o fifo_writer fifo_writer.c
gcc -o fifo_reader fifo_reader.c
```

## Demo Script for Class

### Demo 1: Basic FIFO Creation

```bash
# Step 1: Create the FIFO
./fifo

# Step 2: Show it's a special file (note the 'p' at the start)
ls -l /tmp/my_fifo
# Output: prw-r--r--  1 user  wheel  0 Oct 20 09:52 /tmp/my_fifo
#         ^ This 'p' means it's a pipe!

# Step 3: Show it's a real filesystem entry
file /tmp/my_fifo
# Output: /tmp/my_fifo: fifo (named pipe)
```

### Demo 2: Simple Communication with Shell Commands

**Terminal 1 (Reader):**
```bash
cat /tmp/my_fifo
```
*Point out: This process is now BLOCKED, waiting for data*

**Terminal 2 (Writer):**
```bash
echo "Hello Class!" > /tmp/my_fifo
```
*The message appears in Terminal 1 immediately!*

**Key Teaching Point:** This demonstrates:
- Blocking I/O behavior
- Inter-process communication
- Data flows from writer to reader

### Demo 3: Interactive Chat with Custom Programs

**Terminal 1:**
```bash
./fifo_reader
```

**Terminal 2:**
```bash
./fifo_writer
```

Now type messages in Terminal 2 and watch them appear in Terminal 1!

**Messages to send for demo:**
1. "Hello from process A!"
2. "This is IPC in action!"
3. "quit" (to exit)

## Key Points to Emphasize

1. **Named vs Anonymous Pipes:**
   - Anonymous pipe: `pipe()` - only between related processes
   - Named pipe (FIFO): `mkfifo()` - any processes can use it

2. **Blocking Behavior:**
   - Reader blocks until data is available
   - Writer blocks until reader is ready
   - This is synchronous communication

3. **Filesystem Integration:**
   - FIFO appears as a file (but doesn't use disk space)
   - Survives beyond process lifetime
   - Must be explicitly removed

4. **Use Cases:**
   - Client-server communication on same machine
   - Producer-consumer patterns
   - Command-line tool integration

## Cleanup

```bash
# Remove the FIFO when done
rm /tmp/my_fifo

# Or programmatically:
# unlink("/tmp/my_fifo");
```

## Advanced Demo Ideas

### Multiple Writers, One Reader
Open 3 terminals:
- Terminal 1: Reader
- Terminal 2: Writer 1
- Terminal 3: Writer 2

Both writers can send to the same reader!

### Bidirectional Communication
Create two FIFOs:
- `/tmp/client_to_server` 
- `/tmp/server_to_client`

Each process reads from one and writes to the other.

## Common Questions to Address

**Q: Why use FIFO instead of files?**
A: FIFOs don't store data on disk, they're in-memory. Data flows through them.

**Q: What happens if no reader exists?**
A: Writer blocks until a reader opens the FIFO (try it with the students!).

**Q: Can multiple processes read from one FIFO?**
A: Yes, but data is consumed - only one reader gets each message.

**Q: How is this different from sockets?**
A: FIFOs are simpler but only work on the same machine. Sockets work over networks.

