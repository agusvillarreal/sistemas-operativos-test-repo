# Module 1: System Calls Programming
## Lecture Notes

---

## 📚 Table of Contents
1. [Introduction to System Calls](#introduction)
2. [Kernel vs User Space](#kernel-vs-user-space)
3. [How System Calls Work](#how-system-calls-work)
4. [Common System Calls](#common-system-calls)
5. [Error Handling](#error-handling)
6. [Debugging with Tools](#debugging-tools)
7. [Practical Examples](#practical-examples)

---

## 1. Introduction to System Calls {#introduction}

### What is a System Call?

A **system call** is a programmatic way for a user-space application to request services from the operating system kernel. It's the interface between user programs and the kernel.

**Key Concepts:**
- System calls are the **only** legitimate way to access kernel resources
- They provide a controlled entry point into the kernel
- They switch the CPU from user mode to kernel mode
- They are expensive (compared to function calls) due to context switching

### Why Do We Need System Calls?

```
User Application
       ↓
   System Call (e.g., open(), read())
       ↓
   Kernel Space (privileged mode)
       ↓
   Hardware (disk, network, etc.)
```

**Protection:** The kernel mediates all access to hardware and system resources, preventing:
- Malicious programs from damaging the system
- Bugs in one program from affecting others
- Direct hardware access that could corrupt data

---

## 2. Kernel vs User Space {#kernel-vs-user-space}

### Memory Layout

```
High Memory (0xFFFFFFFF)
┌─────────────────────────┐
│    KERNEL SPACE         │ ← Privileged, ring 0
│  - Kernel code          │
│  - Device drivers       │
│  - Kernel data          │
├─────────────────────────┤ ← System call boundary
│    USER SPACE           │ ← Unprivileged, ring 3
│  - Application code     │
│  - Libraries (libc)     │
│  - Stack                │
│  - Heap                 │
│  - Data                 │
│  - Text (code)          │
└─────────────────────────┘
Low Memory (0x00000000)
```

### Privilege Levels (x86)

| Ring | Privilege | Usage |
|------|-----------|-------|
| 0 | Highest (kernel mode) | OS kernel, device drivers |
| 1-2 | Medium | Rarely used in modern OS |
| 3 | Lowest (user mode) | User applications |

### Transition Process

When a program makes a system call:

1. **User mode** → Prepare arguments
2. **Trap instruction** (INT 0x80 or SYSCALL) → Switch to kernel mode
3. **Kernel mode** → Execute system call handler
4. **Return** → Switch back to user mode
5. **User mode** → Continue execution

**Cost:** ~100-300 CPU cycles for the context switch!

---

## 3. How System Calls Work {#how-system-calls-work}

### System Call Mechanism

#### Step-by-Step Execution:

```c
// User space code
int fd = open("/etc/passwd", O_RDONLY);
```

**What happens internally:**

1. **Wrapper function** (`open()` in libc):
   ```c
   // Simplified wrapper
   int open(const char *pathname, int flags) {
       return syscall(__NR_open, pathname, flags);
   }
   ```

2. **System call number**: Each syscall has a unique number
   - `open()` = syscall #2 (on x86_64)
   - `read()` = syscall #0
   - `write()` = syscall #1

3. **CPU registers**: Arguments passed via registers
   ```
   rax = system call number
   rdi = 1st argument
   rsi = 2nd argument
   rdx = 3rd argument
   r10 = 4th argument
   r8  = 5th argument
   r9  = 6th argument
   ```

4. **Trap to kernel**: `SYSCALL` instruction
   - CPU switches to kernel mode
   - Saves user context
   - Jumps to kernel system call handler

5. **Kernel executes**: 
   ```c
   // Simplified kernel code
   long sys_open(const char __user *filename, int flags, umode_t mode) {
       // 1. Validate parameters
       // 2. Check permissions
       // 3. Perform operation
       // 4. Return file descriptor or error
   }
   ```

6. **Return to user space**:
   - Result placed in `rax` register
   - CPU switches back to user mode
   - Wrapper returns result

### System Call Table

The kernel maintains a **system call table** (syscall_table):

```c
// Simplified kernel code
void *syscall_table[] = {
    [0] = sys_read,
    [1] = sys_write,
    [2] = sys_open,
    [3] = sys_close,
    // ... hundreds more
};
```

---

## 4. Common System Calls {#common-system-calls}

### File Operations

| System Call | Description | Returns |
|-------------|-------------|---------|
| `open()` | Open file/device | File descriptor (int) |
| `read()` | Read from file descriptor | Bytes read (ssize_t) |
| `write()` | Write to file descriptor | Bytes written (ssize_t) |
| `close()` | Close file descriptor | 0 on success |
| `lseek()` | Move file pointer | New offset |
| `stat()` | Get file status | 0 on success |
| `unlink()` | Delete file | 0 on success |

#### Example: open()

```c
#include <fcntl.h>
#include <unistd.h>

int fd = open("/path/to/file", O_RDONLY);
// flags: O_RDONLY, O_WRONLY, O_RDWR
//        O_CREAT, O_APPEND, O_TRUNC, O_NONBLOCK
```

**File descriptor numbers:**
- 0 = STDIN_FILENO
- 1 = STDOUT_FILENO
- 2 = STDERR_FILENO
- 3+ = Your opened files

---

### Process Operations

| System Call | Description | Returns |
|-------------|-------------|---------|
| `fork()` | Create child process | PID in parent, 0 in child |
| `exec*()` | Replace process image | Never returns on success |
| `wait()`/`waitpid()` | Wait for child | Child PID |
| `getpid()` | Get process ID | Current PID |
| `getppid()` | Get parent PID | Parent PID |
| `exit()` | Terminate process | Never returns |
| `kill()` | Send signal | 0 on success |

#### Example: fork()

```c
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        // Error
        perror("fork failed");
    } else if (pid == 0) {
        // Child process
        printf("Child PID: %d\n", getpid());
    } else {
        // Parent process
        printf("Parent PID: %d, Child PID: %d\n", getpid(), pid);
    }
    return 0;
}
```

**Memory behavior after fork():**
- Child gets a **copy** of parent's memory (copy-on-write)
- Both processes run independently
- File descriptors are **shared**

---

### Memory Operations

| System Call | Description | Returns |
|-------------|-------------|---------|
| `mmap()` | Map memory | Pointer to mapped region |
| `munmap()` | Unmap memory | 0 on success |
| `brk()`/`sbrk()` | Change heap size | 0 or pointer |
| `mprotect()` | Change memory protection | 0 on success |

#### Example: mmap()

```c
#include <sys/mman.h>

// Allocate anonymous memory
void *ptr = mmap(NULL, 4096, 
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS,
                 -1, 0);

// Use memory...

munmap(ptr, 4096);
```

---

### Signal Operations

| System Call | Description | Returns |
|-------------|-------------|---------|
| `signal()` | Simple signal handler | Previous handler |
| `sigaction()` | Advanced signal handler | 0 on success |
| `kill()` | Send signal to process | 0 on success |
| `alarm()` | Schedule SIGALRM | Remaining seconds |
| `pause()` | Wait for signal | Always returns -1 |

#### Common Signals:

```c
SIGINT   2   Interrupt (Ctrl+C)
SIGTERM  15  Termination request
SIGKILL  9   Kill (cannot be caught!)
SIGSEGV  11  Segmentation fault
SIGCHLD  17  Child process status changed
SIGALRM  14  Alarm clock
```

---

## 5. Error Handling {#error-handling}

### The errno Variable

When a system call fails, it:
1. Returns -1 (or NULL for pointers)
2. Sets the global variable `errno` to indicate the error

```c
#include <errno.h>
#include <string.h>
#include <stdio.h>

int fd = open("/nonexistent", O_RDONLY);
if (fd == -1) {
    printf("Error number: %d\n", errno);
    printf("Error message: %s\n", strerror(errno));
    perror("open");  // Prints "open: No such file or directory"
}
```

### Common Error Codes

| errno | Macro | Description |
|-------|-------|-------------|
| 1 | EPERM | Operation not permitted |
| 2 | ENOENT | No such file or directory |
| 3 | ESRCH | No such process |
| 4 | EINTR | Interrupted system call |
| 5 | EIO | I/O error |
| 9 | EBADF | Bad file descriptor |
| 11 | EAGAIN | Try again (non-blocking) |
| 12 | ENOMEM | Out of memory |
| 13 | EACCES | Permission denied |
| 14 | EFAULT | Bad address |
| 22 | EINVAL | Invalid argument |

### Best Practices

```c
// ✅ GOOD: Check return value
int fd = open(filename, O_RDONLY);
if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
}

// ❌ BAD: Ignore errors
int fd = open(filename, O_RDONLY);
read(fd, buffer, size);  // fd might be -1!
```

---

## 6. Debugging with Tools {#debugging-tools}

### strace - System Call Tracer

**Purpose:** Trace all system calls made by a program

```bash
# Basic usage
strace ./program

# Trace specific system calls
strace -e trace=open,read,write ./program

# Show timestamps
strace -t ./program

# Follow forks
strace -f ./program

# Count system calls
strace -c ./program

# Save to file
strace -o output.txt ./program
```

**Example Output:**
```
open("/etc/passwd", O_RDONLY)           = 3
read(3, "root:x:0:0:root:/root:/bin/bash\n", 4096) = 1024
close(3)                                = 0
```

**What to look for:**
- Failed system calls (return -1)
- Unexpected system calls
- Performance bottlenecks (many calls)
- File access patterns

---

### ltrace - Library Call Tracer

**Purpose:** Trace library function calls (libc)

```bash
# Basic usage
ltrace ./program

# Trace specific functions
ltrace -e malloc,free ./program

# Show system calls too
ltrace -S ./program
```

**Example Output:**
```
malloc(1024)                            = 0x5555557592a0
strcpy(0x5555557592a0, "Hello")         = 0x5555557592a0
free(0x5555557592a0)                    = <void>
```

---

### gdb - GNU Debugger

**Purpose:** Debug programs, inspect state, set breakpoints

```bash
# Start debugging
gdb ./program

# GDB commands:
(gdb) break main          # Set breakpoint at main
(gdb) run arg1 arg2       # Run with arguments
(gdb) next                # Execute next line
(gdb) step                # Step into function
(gdb) print variable      # Print variable value
(gdb) backtrace           # Show call stack
(gdb) info registers      # Show CPU registers
(gdb) disassemble         # Show assembly code
```

**Debugging System Calls:**
```gdb
(gdb) catch syscall open
(gdb) run
# Stops when open() is called

(gdb) info registers rdi  # 1st argument (filename)
(gdb) info registers rsi  # 2nd argument (flags)
```

---

### valgrind - Memory Debugger

**Purpose:** Detect memory leaks, invalid accesses, uninitialized values

```bash
# Memory leak detection
valgrind --leak-check=full ./program

# Show reachable blocks
valgrind --leak-check=full --show-reachable=yes ./program

# Track file descriptors
valgrind --track-fds=yes ./program
```

**Example Output:**
```
==12345== Invalid read of size 4
==12345==    at 0x400567: main (program.c:23)
==12345==  Address 0x5204050 is 0 bytes after a block of size 16 alloc'd

==12345== LEAK SUMMARY:
==12345==    definitely lost: 1,024 bytes in 1 blocks
```

---

## 7. Practical Examples {#practical-examples}

### Example 1: File Copy (Raw System Calls)

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int fd_src, fd_dst;
    ssize_t bytes_read, bytes_written;
    char buffer[BUFFER_SIZE];
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <dest>\n", argv[0]);
        exit(1);
    }
    
    // Open source file
    fd_src = open(argv[1], O_RDONLY);
    if (fd_src == -1) {
        perror("open source");
        exit(1);
    }
    
    // Create destination file
    fd_dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst == -1) {
        perror("open destination");
        close(fd_src);
        exit(1);
    }
    
    // Copy data
    while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(fd_dst, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("write");
            close(fd_src);
            close(fd_dst);
            exit(1);
        }
    }
    
    if (bytes_read == -1) {
        perror("read");
    }
    
    close(fd_src);
    close(fd_dst);
    
    return 0;
}
```

**To test:**
```bash
gcc -o file_copy file_copy.c
strace ./file_copy source.txt dest.txt
```

---

### Example 2: Process Creation

```c
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0) {
        // Child process
        printf("[Child] PID=%d, PPID=%d\n", getpid(), getppid());
        
        // Execute new program
        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);
        
        // Only reached if exec fails
        perror("execvp");
        exit(1);
    } else {
        // Parent process
        printf("[Parent] PID=%d, Child PID=%d\n", getpid(), pid);
        
        // Wait for child
        int status;
        pid_t child_pid = wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Child %d exited with status %d\n", 
                   child_pid, WEXITSTATUS(status));
        }
    }
    
    return 0;
}
```

---

### Example 3: Signal Handling

```c
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t keep_running = 1;

void signal_handler(int signum) {
    printf("\nReceived signal %d\n", signum);
    keep_running = 0;
}

int main() {
    // Register signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("PID: %d\n", getpid());
    printf("Press Ctrl+C to stop...\n");
    
    int count = 0;
    while (keep_running) {
        printf("Running... %d\r", count++);
        fflush(stdout);
        sleep(1);
    }
    
    printf("\nCleaning up and exiting...\n");
    return 0;
}
```

**Test with:**
```bash
./signal_demo &
kill -SIGTERM $!
```

---

## 📝 Summary

### Key Takeaways:

1. **System calls** are the interface between user programs and kernel
2. They involve **context switches** (expensive operation)
3. Always **check return values** and handle errors
4. Use **strace** to see what your program does
5. Use **gdb** to debug system call issues
6. Use **valgrind** to find memory problems

### System Call Hierarchy:

```
Application Code
      ↓
  C Library (libc) - printf(), malloc()
      ↓
  Wrapper Functions - syscall()
      ↓
  Kernel Interface - SYSCALL instruction
      ↓
  Kernel Handlers - sys_open(), sys_read()
      ↓
  Device Drivers
      ↓
  Hardware
```

---

## 🔍 Additional Resources

- Man pages: `man 2 syscall_name` (e.g., `man 2 open`)
- Linux syscall reference: https://man7.org/linux/man-pages/man2/syscalls.2.html
- strace tutorial: https://strace.io/
- Kernel source code: https://elixir.bootlin.com/linux/latest/source

---

## ❓ Practice Questions

1. What's the difference between `open()` and `fopen()`?
2. Why does `fork()` return twice?
3. What happens if you don't close file descriptors?
4. How can you make a system call "fail gracefully" in a retry loop?
5. What's the difference between SIGKILL and SIGTERM?

---

**Next Module:** Process Analysis & Monitoring

