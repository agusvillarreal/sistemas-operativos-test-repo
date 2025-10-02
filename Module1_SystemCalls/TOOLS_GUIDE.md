# Module 1: System Call Debugging Tools Guide
## Professional Linux Administrator Toolkit

---

## 📚 Table of Contents
1. [strace - System Call Tracer](#strace)
2. [ltrace - Library Call Tracer](#ltrace)
3. [gdb - GNU Debugger](#gdb)
4. [valgrind - Memory Profiler](#valgrind)
5. [proc Filesystem](#proc-filesystem)
6. [lsof - List Open Files](#lsof)
7. [Real-World Examples](#real-world-examples)

---

## 1. strace - System Call Tracer {#strace}

### What is strace?

`strace` traces system calls and signals. It's essential for understanding what your program does at the kernel level.

### Basic Usage

```bash
# Trace all system calls
strace ./program

# Trace specific system calls
strace -e trace=open,read,write ./program

# Follow child processes (crucial for fork analysis)
strace -f ./program

# Save output to file
strace -o trace.log ./program

# Show timestamps
strace -t ./program

# Show time spent in each syscall
strace -T ./program

# Count syscalls (summary)
strace -c ./program

# Attach to running process
strace -p <PID>

# Show string arguments in full
strace -s 1024 ./program
```

### Understanding strace Output

```
open("/etc/passwd", O_RDONLY)           = 3
read(3, "root:x:0:0:root:/root:/bin/bash\n", 4096) = 1024
close(3)                                = 0
```

**Format**: `syscall(arguments) = return_value`

- **Arguments**: Shown with proper formatting
- **Return value**: 
  - Non-negative = success (often fd or bytes)
  - `-1` = error (check errno)
  - Pointer = memory address

### Common Flags Explained

| Flag | Description | Example |
|------|-------------|---------|
| `-e trace=set` | Trace specific syscalls | `-e trace=network` |
| `-e trace=file` | All file operations | `open,stat,access` |
| `-e trace=process` | Process management | `fork,exec,wait` |
| `-e trace=network` | Network operations | `socket,connect,send` |
| `-e trace=signal` | Signal operations | `kill,signal,sigaction` |
| `-e trace=ipc` | IPC operations | `mmap,semop,msgget` |
| `-f` | Follow forks | Essential for multi-process |
| `-ff -o file` | Save each process separately | Creates `file.PID` |
| `-t` | Print timestamp | `HH:MM:SS` |
| `-tt` | Microsecond timestamp | `HH:MM:SS.microseconds` |
| `-r` | Print relative time | Time between calls |
| `-c` | Count time/calls/errors | Summary at end |
| `-p PID` | Attach to running process | Live tracing |
| `-y` | Show file descriptor paths | See what files are open |

### Practical Examples

#### Example 1: Debugging Failed File Open

```bash
$ strace -e trace=open,openat ./program 2>&1 | grep "ENOENT"
openat(AT_FDCWD, "/etc/config.ini", O_RDONLY) = -1 ENOENT (No such file or directory)
```

**Analysis**: Program is looking for `/etc/config.ini` which doesn't exist.

#### Example 2: Finding Performance Bottlenecks

```bash
$ strace -c ./program
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 99.50    0.124987          12     10000           write
  0.30    0.000375          15        25           open
  0.20    0.000251          10        25           close
------ ----------- ----------- --------- --------- ----------------
100.00    0.125613                 10050           total
```

**Analysis**: 99% of time spent in `write()` - I/O bound program.

#### Example 3: Tracing Process Creation

```bash
$ strace -f -e trace=fork,execve,wait4 ./program
[pid 1234] fork()                       = 1235
[pid 1235] execve("/bin/ls", ["ls", "-l"], ...) = 0
[pid 1234] wait4(1235, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 1235
```

**Analysis**: Parent (1234) forks child (1235), child execs `ls`, parent waits.

#### Example 4: Finding Memory Allocation

```bash
$ strace -e trace=mmap,brk ./program
brk(NULL)                               = 0x5555557a2000
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7ffff7fb6000
```

---

## 2. ltrace - Library Call Tracer {#ltrace}

### What is ltrace?

`ltrace` traces library function calls (like `malloc`, `printf`, etc.) from shared libraries.

### Basic Usage

```bash
# Trace all library calls
ltrace ./program

# Trace specific functions
ltrace -e malloc,free ./program

# Show system calls too
ltrace -S ./program

# Count calls
ltrace -c ./program

# Follow forks
ltrace -f ./program

# Save to file
ltrace -o trace.log ./program
```

### Example Output

```
malloc(1024)                            = 0x5555557592a0
strcpy(0x5555557592a0, "Hello World")   = 0x5555557592a0
printf("Result: %s\n", "Hello World")   = 15
free(0x5555557592a0)                    = <void>
```

### Combining strace and ltrace

```bash
# See both library calls and system calls
ltrace -S ./program

# Or run both:
strace -o strace.log ./program
ltrace -o ltrace.log ./program
```

---

## 3. gdb - GNU Debugger {#gdb}

### What is gdb?

`gdb` is a powerful debugger for C/C++ programs. It lets you step through code, inspect variables, and analyze crashes.

### Compilation for Debugging

```bash
# Compile with debug symbols (-g)
gcc -g -o program program.c

# Disable optimizations for better debugging
gcc -g -O0 -o program program.c
```

### Basic Commands

```bash
# Start debugging
gdb ./program

# Common GDB commands:
(gdb) help              # Show help
(gdb) run [args]        # Run program
(gdb) break main        # Set breakpoint at main
(gdb) break file.c:42   # Set breakpoint at line 42
(gdb) info breakpoints  # List breakpoints
(gdb) delete 1          # Delete breakpoint 1
(gdb) step              # Step into function (s)
(gdb) next              # Step over function (n)
(gdb) continue          # Continue execution (c)
(gdb) finish            # Run until function returns
(gdb) print var         # Print variable value (p)
(gdb) print *ptr        # Dereference pointer
(gdb) display var       # Auto-print variable
(gdb) backtrace         # Show call stack (bt)
(gdb) frame 2           # Switch to frame 2
(gdb) info locals       # Show local variables
(gdb) info registers    # Show CPU registers
(gdb) disassemble       # Show assembly code
(gdb) quit              # Exit gdb (q)
```

### Advanced Commands

```bash
# Conditional breakpoint
(gdb) break file.c:42 if x == 10

# Watch variable changes
(gdb) watch variable_name

# Catch system calls
(gdb) catch syscall open

# Attach to running process
gdb -p <PID>

# Examine memory
(gdb) x/10x 0x7fffffffe000  # 10 hex words
(gdb) x/s ptr                # String at pointer

# Change variable value
(gdb) set var x = 100

# Call function
(gdb) call printf("Debug: %d\n", x)
```

### Debugging System Calls with gdb

```bash
$ gdb ./program
(gdb) catch syscall open
(gdb) run
Catchpoint 1 (call to syscall open), 0x00007ffff7b08117 in open64 ()

(gdb) info registers rdi    # 1st arg: filename
(gdb) x/s $rdi
0x7fffffffe800: "/etc/passwd"

(gdb) info registers rsi    # 2nd arg: flags
(gdb) print /x $rsi
$1 = 0x0    # O_RDONLY

(gdb) continue
```

### Debugging Fork

```bash
# Follow parent or child after fork
(gdb) set follow-fork-mode parent  # Default
(gdb) set follow-fork-mode child

# Detach when forking
(gdb) set detach-on-fork off

# See all processes
(gdb) info inferiors
  Num  Description       Executable
* 1    process 1234      /path/to/program
  2    process 1235      /path/to/program

# Switch between processes
(gdb) inferior 2
```

### Core Dump Analysis

```bash
# Enable core dumps
ulimit -c unlimited

# Program crashes, creates core file
./program
Segmentation fault (core dumped)

# Analyze core dump
gdb ./program core
(gdb) backtrace        # See where it crashed
(gdb) frame 0          # Go to crash frame
(gdb) print variable   # Inspect variables
```

---

## 4. valgrind - Memory Profiler {#valgrind}

### What is valgrind?

`valgrind` detects memory leaks, invalid accesses, and uninitialized values.

### Basic Usage

```bash
# Memory leak detection
valgrind --leak-check=full ./program

# Show reachable blocks (still allocated)
valgrind --leak-check=full --show-reachable=yes ./program

# Track file descriptor leaks
valgrind --track-fds=yes ./program

# Show origins of uninitialized values
valgrind --track-origins=yes ./program

# Generate detailed report
valgrind --leak-check=full --log-file=valgrind.log ./program

# More verbose output
valgrind -v --leak-check=full ./program
```

### Understanding valgrind Output

```
==12345== Invalid read of size 4
==12345==    at 0x400567: main (program.c:23)
==12345==  Address 0x5204050 is 0 bytes after a block of size 16 alloc'd
==12345==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/...)
==12345==    by 0x400557: main (program.c:20)

==12345== LEAK SUMMARY:
==12345==    definitely lost: 1,024 bytes in 1 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==      possibly lost: 0 bytes in 0 blocks
==12345==    still reachable: 0 bytes in 0 blocks
```

**Types of leaks:**
- **Definitely lost**: Memory that's leaked (you lost the pointer)
- **Indirectly lost**: Memory pointed to by leaked blocks
- **Possibly lost**: Pointers found, but not to the start of block
- **Still reachable**: Memory still pointed to but not freed

### Common Memory Errors Detected

1. **Use after free**
2. **Double free**
3. **Buffer overflows**
4. **Uninitialized memory reads**
5. **File descriptor leaks**
6. **Memory leaks**

---

## 5. /proc Filesystem {#proc-filesystem}

### What is /proc?

`/proc` is a pseudo-filesystem that provides interface to kernel data structures.

### Important Files

```bash
/proc/[pid]/cmdline    # Command line arguments
/proc/[pid]/environ    # Environment variables
/proc/[pid]/exe        # Symlink to executable
/proc/[pid]/fd/        # Open file descriptors
/proc/[pid]/maps       # Memory mappings
/proc/[pid]/stat       # Process status
/proc/[pid]/status     # Human-readable status
/proc/[pid]/io         # I/O statistics
/proc/[pid]/limits     # Resource limits
/proc/[pid]/cwd        # Current working directory
/proc/cpuinfo          # CPU information
/proc/meminfo          # Memory information
/proc/sys/             # Kernel parameters
```

### Reading Process Info

```c
// Read process status
char path[256];
sprintf(path, "/proc/%d/stat", pid);

FILE *fp = fopen(path, "r");
int pid, ppid;
char comm[256], state;
unsigned long utime, stime;

fscanf(fp, "%d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
       &pid, comm, &state, &ppid, &utime, &stime);

fclose(fp);

printf("PID: %d, Command: %s, State: %c, PPID: %d\n", 
       pid, comm, state, ppid);
```

### Process State Codes

```
R  Running
S  Sleeping (interruptible)
D  Disk sleep (uninterruptible)
Z  Zombie
T  Stopped (on a signal)
t  Tracing stop
X  Dead
```

---

## 6. lsof - List Open Files {#lsof}

### What is lsof?

`lsof` lists open files and the processes that opened them.

### Basic Usage

```bash
# List all open files
lsof

# Files opened by specific process
lsof -p <PID>

# Processes using specific file
lsof /path/to/file

# Network connections
lsof -i
lsof -i :8080           # Port 8080
lsof -i TCP:22          # SSH connections

# Files in directory
lsof +D /var/log

# User's open files
lsof -u username

# All except user
lsof -u ^root
```

### Example Output

```
COMMAND   PID USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
program  1234 user  cwd    DIR    8,1     4096  123 /home/user
program  1234 user  txt    REG    8,1    12345  456 /usr/bin/program
program  1234 user    0u   CHR  136,0      0t0    3 /dev/pts/0
program  1234 user    1u   CHR  136,0      0t0    3 /dev/pts/0
program  1234 user    2u   CHR  136,0      0t0    3 /dev/pts/0
program  1234 user    3r   REG    8,1      100  789 /etc/passwd
```

**Columns:**
- **FD**: File descriptor (0=stdin, 1=stdout, 2=stderr)
- **TYPE**: File type (REG=regular, DIR=directory, CHR=character device)
- **r/w/u**: Read/write/read-write mode

---

## 7. Real-World Examples {#real-world-examples}

### Scenario 1: Program Fails to Start

```bash
# Check what it's trying to open
strace -e trace=open,openat ./program 2>&1 | grep -i error

# Check library dependencies
ldd ./program

# Trace library loading
LD_DEBUG=libs ./program
```

### Scenario 2: Memory Leak Hunting

```bash
# 1. Detect leaks
valgrind --leak-check=full --log-file=leak.log ./program

# 2. Analyze specific allocation
valgrind --leak-check=full --show-leak-kinds=all ./program

# 3. Find where it was allocated
# (line numbers in leak.log point to source)
```

### Scenario 3: Process Hangs

```bash
# 1. Find the process
ps aux | grep program

# 2. See what it's doing
strace -p <PID>

# 3. Check open files
lsof -p <PID>

# 4. See what it's waiting for
cat /proc/<PID>/wchan    # Wait channel

# 5. Attach debugger
gdb -p <PID>
(gdb) backtrace
```

### Scenario 4: Zombie Processes

```bash
# 1. Find zombies
ps aux | grep 'Z'

# 2. Find parent
ps -o ppid= -p <zombie_PID>

# 3. Check what parent is doing
strace -p <parent_PID>

# 4. Is parent handling SIGCHLD?
cat /proc/<parent_PID>/status | grep SigCgt
```

### Scenario 5: High CPU Usage

```bash
# 1. Find culprit
top
pidstat 1

# 2. Trace system calls
strace -c -p <PID>

# 3. Profile with perf
perf record -p <PID>
perf report
```

---

## 🎓 Practice Exercises

### Exercise 1: File Operations
```bash
# Create a program that reads /etc/passwd
gcc -o read_passwd read_passwd.c

# Trace it with strace
strace -e trace=open,read,close ./read_passwd

# Questions:
# - How many read() calls are made?
# - What's the buffer size used?
# - How many bytes are read total?
```

### Exercise 2: Memory Debugging
```bash
# Create a program with intentional memory leak
# Use valgrind to find it
# Fix the leak and verify with valgrind
```

### Exercise 3: Process Analysis
```bash
# Create a program that forks 5 children
# Use strace -f to trace all processes
# Draw the process tree from the trace
```

---

## 📚 Quick Reference

### One-Liner Cheat Sheet

```bash
# System calls for file operations
strace -e trace=file ./program

# Follow all forks
strace -f -o trace.log ./program

# Attach to running process
strace -p $(pgrep program_name)

# Memory leak check
valgrind --leak-check=full ./program

# Debug with gdb
gdb -ex "break main" -ex "run" ./program

# Find what files a process has open
lsof -p <PID>

# See process tree
pstree -p <PID>

# Monitor process
pidstat -p <PID> 1
```

---

**Master these tools and you'll be a debugging wizard!** 🧙‍♂️

