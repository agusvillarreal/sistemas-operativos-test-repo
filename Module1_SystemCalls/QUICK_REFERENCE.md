# Module 1: Quick Reference Sheet
## System Calls Programming

---

## 🔧 System Calls Cheat Sheet

### File Operations

```c
#include <fcntl.h>
#include <unistd.h>

// Open file
int fd = open(path, O_RDONLY);              // Read only
int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Write, create, truncate

// Flags:
// O_RDONLY, O_WRONLY, O_RDWR   - Access modes
// O_CREAT                       - Create if doesn't exist
// O_TRUNC                       - Truncate to zero length
// O_APPEND                      - Append to end
// O_NONBLOCK                    - Non-blocking I/O

// Read/Write
ssize_t bytes = read(fd, buffer, size);
ssize_t bytes = write(fd, buffer, size);

// Seek
off_t offset = lseek(fd, 0, SEEK_SET);      // Beginning
off_t offset = lseek(fd, 0, SEEK_END);      // End
off_t offset = lseek(fd, 10, SEEK_CUR);     // Current + 10

// Close
close(fd);

// File info
struct stat sb;
stat(path, &sb);                             // By path
fstat(fd, &sb);                              // By file descriptor

// Delete
unlink(path);
```

### Process Operations

```c
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Fork
pid_t pid = fork();
if (pid == 0) {
    // Child process
} else if (pid > 0) {
    // Parent process (pid = child's PID)
} else {
    // Error
}

// Execute
char *args[] = {"ls", "-l", NULL};
execvp("ls", args);                          // Search PATH
execv("/bin/ls", args);                      // Full path
execve("/bin/ls", args, envp);               // With environment

// Wait for child
int status;
pid_t child_pid = wait(&status);             // Wait for any child
pid_t child_pid = waitpid(pid, &status, 0);  // Wait for specific child
pid_t child_pid = waitpid(-1, &status, WNOHANG);  // Non-blocking

// Check exit status
if (WIFEXITED(status)) {
    int exit_code = WEXITSTATUS(status);
}
if (WIFSIGNALED(status)) {
    int signal = WTERMSIG(status);
}

// Process info
pid_t pid = getpid();                        // My PID
pid_t ppid = getppid();                      // Parent's PID
pid_t pgid = getpgrp();                      // Process group ID

// Exit
exit(0);                                     // Clean exit
_exit(0);                                    // Immediate exit (no cleanup)
```

### Signal Operations

```c
#include <signal.h>

// Simple handler
void handler(int signum) {
    // Handle signal (limited functions allowed!)
}
signal(SIGINT, handler);

// Advanced handler
struct sigaction sa;
sa.sa_handler = handler;
sa.sa_flags = 0;
sigemptyset(&sa.sa_mask);
sigaction(SIGINT, &sa, NULL);

// Send signal
kill(pid, SIGUSR1);                          // To process
kill(-pgid, SIGTERM);                        // To process group

// Signal set operations
sigset_t set;
sigemptyset(&set);                           // Clear all
sigfillset(&set);                            // Set all
sigaddset(&set, SIGINT);                     // Add signal
sigdelset(&set, SIGINT);                     // Remove signal

// Block/unblock signals
sigprocmask(SIG_BLOCK, &set, NULL);          // Block
sigprocmask(SIG_UNBLOCK, &set, NULL);        // Unblock

// Wait for signal
pause();                                     // Wait for any signal
sigwait(&set, &sig);                         // Wait for specific signal

// Alarm
alarm(5);                                    // SIGALRM in 5 seconds
```

### Memory Operations

```c
#include <sys/mman.h>
#include <stdlib.h>

// Anonymous memory
void *ptr = mmap(NULL, size, 
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS,
                 -1, 0);

// File-backed memory
int fd = open("file", O_RDWR);
void *ptr = mmap(NULL, size,
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED,
                 fd, 0);

// Unmap
munmap(ptr, size);

// Change protection
mprotect(ptr, size, PROT_READ);              // Make read-only

// Heap
void *ptr = malloc(size);                    // Allocate
free(ptr);                                   // Free
```

---

## 📊 Common Error Codes

```c
#include <errno.h>
#include <string.h>

// Check error
if (syscall() == -1) {
    perror("syscall");                       // Print error
    printf("Error: %s\n", strerror(errno));  // Get error string
    printf("Error code: %d\n", errno);       // Get error number
}
```

| errno | Name | Description |
|-------|------|-------------|
| 1 | EPERM | Operation not permitted |
| 2 | ENOENT | No such file or directory |
| 3 | ESRCH | No such process |
| 4 | EINTR | Interrupted system call |
| 9 | EBADF | Bad file descriptor |
| 11 | EAGAIN | Resource temporarily unavailable |
| 12 | ENOMEM | Out of memory |
| 13 | EACCES | Permission denied |
| 14 | EFAULT | Bad address |
| 17 | EEXIST | File exists |
| 22 | EINVAL | Invalid argument |
| 24 | EMFILE | Too many open files |
| 32 | EPIPE | Broken pipe |

---

## 🎯 Common Signals

| Signal | Number | Default | Description |
|--------|--------|---------|-------------|
| SIGHUP | 1 | Term | Hangup |
| SIGINT | 2 | Term | Interrupt (Ctrl+C) |
| SIGQUIT | 3 | Core | Quit (Ctrl+\\) |
| SIGILL | 4 | Core | Illegal instruction |
| SIGABRT | 6 | Core | Abort |
| SIGFPE | 8 | Core | Floating-point exception |
| SIGKILL | 9 | Term | Kill (cannot catch!) |
| SIGSEGV | 11 | Core | Segmentation fault |
| SIGPIPE | 13 | Term | Broken pipe |
| SIGALRM | 14 | Term | Alarm clock |
| SIGTERM | 15 | Term | Termination request |
| SIGCHLD | 17 | Ign | Child terminated |
| SIGCONT | 18 | Cont | Continue if stopped |
| SIGSTOP | 19 | Stop | Stop (cannot catch!) |
| SIGUSR1 | 10 | Term | User-defined 1 |
| SIGUSR2 | 12 | Term | User-defined 2 |

---

## 🛠️ Debugging Tools

### strace

```bash
# Trace all syscalls
strace ./program

# Trace specific syscalls
strace -e trace=open,read,write ./program

# Follow forks
strace -f ./program

# Count syscalls
strace -c ./program

# Save to file
strace -o trace.log ./program

# Attach to running process
strace -p <PID>
```

### gdb

```bash
# Start debugging
gdb ./program

# Commands:
break main              # Set breakpoint
run [args]              # Run program
next                    # Step over (n)
step                    # Step into (s)
continue                # Continue (c)
print var               # Print variable (p)
backtrace               # Call stack (bt)
info locals             # Local variables
quit                    # Exit (q)
```

### valgrind

```bash
# Memory leak detection
valgrind --leak-check=full ./program

# Track file descriptors
valgrind --track-fds=yes ./program

# Show origins
valgrind --track-origins=yes ./program
```

---

## 📁 /proc Filesystem

```bash
/proc/[pid]/cmdline     # Command line
/proc/[pid]/exe         # Executable path
/proc/[pid]/fd/         # Open file descriptors
/proc/[pid]/stat        # Process status
/proc/[pid]/status      # Human-readable status
/proc/[pid]/maps        # Memory mappings
```

### Reading Process Info (C)

```c
char path[256];
sprintf(path, "/proc/%d/stat", pid);

FILE *fp = fopen(path, "r");
int pid, ppid;
char comm[256], state;

fscanf(fp, "%d %s %c %d", &pid, comm, &state, &ppid);
fclose(fp);
```

---

## 💡 Common Patterns

### Error Handling

```c
if (syscall() == -1) {
    perror("syscall");
    exit(EXIT_FAILURE);
}
```

### Fork-Exec Pattern

```c
pid_t pid = fork();
if (pid == 0) {
    // Child
    execvp(command, args);
    perror("exec failed");
    exit(1);
} else {
    // Parent
    int status;
    waitpid(pid, &status, 0);
}
```

### Signal-Safe Handler

```c
volatile sig_atomic_t flag = 0;

void handler(int signum) {
    // Only async-signal-safe functions!
    const char msg[] = "Signal received\n";
    write(STDOUT_FILENO, msg, sizeof(msg)-1);
    flag = 1;
}
```

### Prevent Zombies

```c
// Method 1: Wait for children
int status;
while (waitpid(-1, &status, WNOHANG) > 0);

// Method 2: Ignore SIGCHLD
signal(SIGCHLD, SIG_IGN);

// Method 3: SIGCHLD handler
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
signal(SIGCHLD, sigchld_handler);
```

### Read Entire File

```c
int fd = open(path, O_RDONLY);
if (fd == -1) {
    perror("open");
    return -1;
}

char buffer[4096];
ssize_t bytes;
while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
    // Process buffer
    write(STDOUT_FILENO, buffer, bytes);
}

if (bytes == -1) {
    perror("read");
}

close(fd);
```

---

## 🚦 Best Practices

### ✅ DO

- Always check return values
- Close file descriptors when done
- Free allocated memory
- Use `perror()` for error messages
- Block signals during critical sections
- Use `const` for string literals
- Initialize variables
- Comment complex code

### ❌ DON'T

- Ignore return values
- Use `printf()` in signal handlers
- Access memory after `free()`
- Forget to `wait()` for children
- Use uninitialized variables
- Hardcode buffer sizes
- Mix tabs and spaces
- Leave debug code in production

---

## 📖 Man Pages Reference

```bash
man 2 fork      # System calls (section 2)
man 3 printf    # Library functions (section 3)
man 5 proc      # File formats (section 5)
man 7 signal    # Overviews (section 7)

# Search man pages
man -k keyword
apropos keyword
```

---

## 🔍 Common Commands

```bash
# Process monitoring
ps aux                  # All processes
ps -ef                  # Full format
top                     # Interactive monitor
htop                    # Better top
pstree -p               # Process tree

# Process control
kill -l                 # List signals
kill -TERM <pid>        # Terminate
kill -KILL <pid>        # Force kill
killall program         # Kill by name

# File descriptors
lsof -p <pid>           # Files opened by process
lsof /path/to/file      # Processes using file
lsof -i :8080           # Processes on port 8080

# System info
uname -a                # System info
cat /proc/cpuinfo       # CPU info
cat /proc/meminfo       # Memory info
free -h                 # Memory usage
df -h                   # Disk usage
```

---

## 🎓 Study Tips

1. **Practice with examples**: Run the code in `Module1_SystemCalls/examples/`
2. **Use strace**: Trace everything to see what's happening
3. **Read man pages**: `man 2 syscall_name`
4. **Debug with gdb**: Step through code line by line
5. **Check with valgrind**: Find memory issues early
6. **Test error cases**: What happens when syscalls fail?
7. **Experiment**: Try different parameters and flags

---

**Print this reference sheet for quick access during coding!** 📄

