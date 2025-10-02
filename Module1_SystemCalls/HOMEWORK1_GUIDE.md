# Homework 1: Basic Process Management in C
## Standalone Assignment - System Calls & Process Management

**Due Date:** 2 weeks from assignment date  
**Weight:** 15% of final grade  
**Points:** 100 points  
**Submission:** Source code + README + Makefile + test cases

---

## 📌 Assignment Overview

This is **Homework 1 of 5** in the Linux Systems Programming course. Each homework is an **independent project** focusing on different aspects of operating systems:

- **Homework 1** (This): Basic Process Management ← You are here
- **Homework 2**: Zombie Process Handler
- **Homework 3**: Scheduler Simulator with GUI  
- **Homework 4**: Chat System using IPC
- **Homework 5**: Custom Kernel Module

---

## 🎯 Learning Objectives

By completing this homework, you will:
1. Master fork(), exec(), wait(), and signal handling
2. Implement a process manager using system calls
3. Handle process lifecycle and zombie processes
4. Visualize process hierarchies
5. Use debugging tools (strace, gdb, valgrind)

---

## 📋 Assignment Overview

You will implement a **Process Manager** in C that can:
- Create and manage multiple child processes
- Execute external programs
- Monitor process status
- Handle signals for graceful shutdown
- Visualize the process tree
- Prevent zombie processes

---

## 🔧 Requirements

### Part 1: Basic Process Creation (25 points)

Implement a program `procman.c` with the following functions:

#### 1.1 Process Creation (10 points)
```c
/**
 * Creates a child process and executes the given command
 * Returns: child PID on success, -1 on failure
 */
int create_process(const char *command, char *args[]);
```

**Requirements:**
- Use `fork()` to create child process
- Use `execvp()` to execute command
- Parent should store child PID
- Handle errors properly

**Example:**
```c
char *args[] = {"ls", "-l", NULL};
pid_t pid = create_process("ls", args);
```

#### 1.2 Process Status Monitoring (10 points)
```c
/**
 * Checks if a process is still running
 * Returns: 1 if running, 0 if terminated, -1 on error
 */
int check_process_status(pid_t pid);
```

**Requirements:**
- Use `waitpid()` with `WNOHANG` option
- Don't block if process is still running
- Report exit status if terminated

#### 1.3 Process Termination (5 points)
```c
/**
 * Terminates a process gracefully (SIGTERM) or forcefully (SIGKILL)
 * Returns: 0 on success, -1 on failure
 */
int terminate_process(pid_t pid, int force);
```

**Requirements:**
- Send `SIGTERM` if `force` is 0
- Send `SIGKILL` if `force` is 1
- Wait for process termination
- Clean up zombie processes

---

### Part 2: Process Manager (30 points)

Implement a process manager that maintains a table of running processes:

#### 2.1 Process Table (10 points)
```c
#define MAX_PROCESSES 10

typedef struct {
    pid_t pid;
    char command[256];
    time_t start_time;
    int status;  // 0=running, 1=terminated, -1=error
} process_info_t;

process_info_t process_table[MAX_PROCESSES];
int process_count;
```

**Requirements:**
- Initialize table on startup
- Add process to table when created
- Update status when processes terminate
- Remove terminated processes from table

#### 2.2 Process List (10 points)
```c
/**
 * Prints information about all managed processes
 */
void list_processes(void);
```

**Output Format:**
```
PID     COMMAND         RUNTIME    STATUS
-----   -------------   --------   ----------
1234    sleep 100       00:00:15   Running
1235    ls -l           00:00:00   Terminated
```

#### 2.3 Process Wait (10 points)
```c
/**
 * Waits for all child processes to complete
 */
void wait_all_processes(void);
```

**Requirements:**
- Wait for each process in table
- Handle processes that have already terminated
- Update process table accordingly
- Print summary when done

---

### Part 3: Signal Handling (20 points)

Implement signal handlers for graceful shutdown:

#### 3.1 SIGINT Handler (Ctrl+C) (10 points)
```c
void sigint_handler(int signum);
```

**Requirements:**
- Print "Shutting down gracefully..."
- Send `SIGTERM` to all child processes
- Wait for all children to terminate
- Exit cleanly

#### 3.2 SIGCHLD Handler (10 points)
```c
void sigchld_handler(int signum);
```

**Requirements:**
- Automatically reap terminated children
- Update process table
- Prevent zombie processes
- Handle multiple simultaneous child terminations

---

### Part 4: Process Tree Visualization (15 points)

Implement a function to visualize the process hierarchy:

```c
/**
 * Prints process tree starting from given PID
 */
void print_process_tree(pid_t root_pid, int depth);
```

**Output Format:**
```
[1000] procman
  ├─ [1001] sleep 30
  ├─ [1002] child_worker
  │   └─ [1003] grandchild
  └─ [1004] sleep 60
```

**Requirements:**
- Read from `/proc` filesystem
- Show parent-child relationships
- Use proper tree formatting (├─ └─)
- Recursive traversal

**Hint:** Read `/proc/[pid]/stat` to get parent PID (PPID)

---

### Part 5: Interactive Shell (10 points)

Create an interactive command-line interface:

```
ProcMan> help
Available commands:
  create <command> [args...] - Create new process
  list                       - List all processes
  kill <pid> [force]         - Terminate process
  tree                       - Show process tree
  wait                       - Wait for all processes
  quit                       - Exit program

ProcMan> create sleep 30
Created process 1234

ProcMan> list
PID     COMMAND         RUNTIME    STATUS
-----   -------------   --------   ----------
1234    sleep 30        00:00:05   Running

ProcMan> tree
[1000] procman
  └─ [1234] sleep 30

ProcMan> quit
Shutting down...
```

---

## 📦 Deliverables

Your submission must include:

### 1. Source Code Files

```
homework1/
├── procman.c           # Main program
├── procman.h           # Header file
├── Makefile            # Build script
├── README.md           # Documentation
└── test_scripts/       # Test cases
    ├── test1.sh
    ├── test2.sh
    └── test3.sh
```

### 2. README.md

Must include:
- **Author Information**: Name, student ID, email
- **Compilation Instructions**: How to build
- **Usage Instructions**: How to run
- **Design Decisions**: Why you made certain choices
- **Challenges**: Problems faced and solutions
- **Testing**: How you tested the program
- **Known Limitations**: Any bugs or missing features

### 3. Makefile

Must support:
```bash
make              # Build program
make clean        # Remove binaries
make test         # Run test suite
make valgrind     # Check memory leaks
make strace       # Trace system calls
```

### 4. Test Cases

Provide at least 3 test scripts:
- **test1.sh**: Basic process creation and termination
- **test2.sh**: Signal handling (SIGINT, SIGCHLD)
- **test3.sh**: Process tree with multiple levels

---

## 🧪 Testing Requirements

### Basic Tests

1. **Create single process**:
   ```bash
   ./procman
   ProcMan> create sleep 5
   ProcMan> list
   ProcMan> wait
   ```

2. **Create multiple processes**:
   ```bash
   ./procman
   ProcMan> create sleep 30
   ProcMan> create sleep 60
   ProcMan> list
   ProcMan> quit
   ```

3. **Kill process**:
   ```bash
   ./procman
   ProcMan> create sleep 100
   ProcMan> kill 1234
   ProcMan> list
   ```

4. **Signal handling**:
   ```bash
   ./procman
   ProcMan> create sleep 30
   # Press Ctrl+C (should shutdown gracefully)
   ```

### Advanced Tests

5. **Process tree**:
   ```bash
   ./procman
   ProcMan> create bash -c "sleep 30 & sleep 60 & wait"
   ProcMan> tree
   ```

6. **Zombie prevention**:
   ```bash
   ./procman
   ProcMan> create bash -c "exit 0"
   # Check for zombies: ps aux | grep Z
   ```

7. **Memory leaks**:
   ```bash
   valgrind --leak-check=full ./procman
   ProcMan> create sleep 5
   ProcMan> wait
   ProcMan> quit
   ```

---

## 📊 Grading Rubric

| Component | Points | Criteria |
|-----------|--------|----------|
| **Part 1: Basic Process Creation** | 25 | |
| - create_process() | 10 | Correctly uses fork/exec |
| - check_process_status() | 10 | Properly uses waitpid() |
| - terminate_process() | 5 | Sends signals correctly |
| **Part 2: Process Manager** | 30 | |
| - Process table | 10 | Manages processes correctly |
| - list_processes() | 10 | Shows all info, formatted |
| - wait_all_processes() | 10 | Waits for all children |
| **Part 3: Signal Handling** | 20 | |
| - SIGINT handler | 10 | Graceful shutdown |
| - SIGCHLD handler | 10 | Auto-reaps zombies |
| **Part 4: Process Tree** | 15 | |
| - Tree visualization | 15 | Correct tree structure |
| **Part 5: Interactive Shell** | 10 | |
| - Command parsing | 5 | Handles all commands |
| - User interface | 5 | Clean, user-friendly |
| **Code Quality** | 10 | |
| - Comments | 3 | Clear documentation |
| - Error handling | 3 | Checks all return values |
| - Code style | 2 | Consistent formatting |
| - No memory leaks | 2 | Valgrind clean |
| **Documentation** | 10 | |
| - README | 5 | Complete, clear |
| - Test scripts | 3 | Comprehensive tests |
| - Makefile | 2 | All targets work |
| **BONUS** | +10 | |
| - Advanced features | +5 | CPU/memory monitoring |
| - GUI | +5 | ncurses interface |

---

## 💡 Hints and Tips

### Using /proc Filesystem

```c
// Read process information
char path[256];
sprintf(path, "/proc/%d/stat", pid);

FILE *fp = fopen(path, "r");
if (fp) {
    int pid;
    char comm[256];
    char state;
    int ppid;
    
    fscanf(fp, "%d %s %c %d", &pid, comm, &state, &ppid);
    fclose(fp);
}
```

### Preventing Race Conditions

```c
// Block signals during critical section
sigset_t mask, oldmask;
sigemptyset(&mask);
sigaddset(&mask, SIGCHLD);

sigprocmask(SIG_BLOCK, &mask, &oldmask);

// Critical section: modify process_table

sigprocmask(SIG_SETMASK, &oldmask, NULL);
```

### Error Handling Pattern

```c
if (fork() == -1) {
    perror("fork");
    fprintf(stderr, "Failed to create process: %s\n", strerror(errno));
    return -1;
}
```

### Debugging with GDB

```bash
# Compile with debug symbols
gcc -g -o procman procman.c

# Debug
gdb ./procman
(gdb) break create_process
(gdb) run
(gdb) next
(gdb) print pid
(gdb) continue
```

### Tracing with strace

```bash
# Trace process-related syscalls
strace -f -e trace=fork,execve,wait4,kill ./procman

# Follow child processes
strace -f -o trace.log ./procman
```

---

## 📚 Resources

### Man Pages
```bash
man 2 fork
man 2 execve
man 2 wait
man 2 waitpid
man 2 kill
man 2 signal
man 2 sigaction
man 5 proc
```

### Recommended Reading
- **OSTEP Chapter 5**: Process API
- **OSTEP Chapter 26**: Concurrency: An Introduction
- **The Linux Programming Interface**: Chapters 24-27

### Example Code
- See `Module1_SystemCalls/examples/` directory
- Study `02_process_creation.c` for fork/exec patterns
- Study `03_signal_handling.c` for signal handling

---

## ⚠️ Common Mistakes to Avoid

1. **Forgetting to reap children** → Zombie processes
2. **Not checking return values** → Silent failures
3. **Using printf() in signal handlers** → Not async-signal-safe
4. **Ignoring EINTR errors** → Lost signals
5. **Memory leaks** → Not freeing allocated memory
6. **Race conditions** → Not blocking signals during updates
7. **Hardcoded buffer sizes** → Buffer overflows

---

## 🏆 Bonus Challenges (+10 points)

### Bonus 1: Resource Monitoring (+5 points)
Add CPU and memory usage monitoring for each process:
```
PID     COMMAND         CPU%    MEM%    RUNTIME
-----   -------------   ------  ------  --------
1234    sleep 30        0.0%    0.1%    00:00:15
```

Read from `/proc/[pid]/stat` and `/proc/[pid]/statm`

### Bonus 2: ncurses GUI (+5 points)
Create a terminal UI using ncurses library:
- Live updating process list
- Color-coded status (green=running, red=terminated)
- Keyboard shortcuts (k=kill, c=create, q=quit)

---

## 📝 Submission Instructions

1. **Create a tarball**:
   ```bash
   tar -czf homework1_lastname_firstname.tar.gz homework1/
   ```

2. **Verify contents**:
   ```bash
   tar -tzf homework1_lastname_firstname.tar.gz
   ```

3. **Submit via course portal** by the due date

4. **Late Policy**: -10% per day, max 3 days late

---

## ❓ Frequently Asked Questions

**Q: Can I use C++ instead of C?**  
A: No, this must be pure C to learn system-level programming.

**Q: Can I use pthreads?**  
A: No, this assignment is about processes, not threads.

**Q: How do I test SIGCHLD handler?**  
A: Create a short-lived process and verify it's reaped without explicit wait().

**Q: What if execvp() fails?**  
A: Child should print error and call `exit(EXIT_FAILURE)`.

**Q: Can I use external libraries?**  
A: Only standard C library and system calls. No third-party libraries.

---

## 📞 Getting Help

- **Office Hours**: Check syllabus for times
- **Discussion Forum**: Post questions (don't share code!)
- **Email**: For private questions only
- **Study Groups**: Encouraged, but write your own code

---

**Good luck! Start early and test thoroughly!** 🚀

