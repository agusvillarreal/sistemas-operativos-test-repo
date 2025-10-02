# Homework 2: Zombie Process Handler
## Standalone Assignment - Process Lifecycle & Zombie Management

**Due Date:** 2 weeks from assignment date  
**Weight:** 15% of final grade  
**Points:** 100 points  
**Submission:** Source code + README + Makefile + test cases + demo video

---

## 📌 Assignment Overview

This is **Homework 2 of 5** in the Linux Systems Programming course. Each homework is an **independent project**:

- Homework 1: Basic Process Management
- **Homework 2** (This): Zombie Process Handler ← You are here
- Homework 3: Scheduler Simulator with GUI  
- Homework 4: Chat System using IPC
- Homework 5: Custom Kernel Module

---

## 🎯 Learning Objectives

By completing this homework, you will:
1. Understand zombie process creation and implications
2. Implement automatic zombie detection and reaping
3. Use SIGCHLD signal handler for async reaping
4. Monitor process states using /proc filesystem
5. Create a robust process monitor tool

---

## 📋 Assignment Description

You will implement a **Zombie Process Monitor and Reaper** that can:
- Intentionally create zombie processes for testing
- Automatically detect zombie processes on the system
- Reap zombie processes using multiple strategies
- Monitor and report process state changes
- Prevent zombie accumulation in long-running applications

---

## 🔧 Requirements

### Part 1: Zombie Creator (20 points)

Implement a program `zombie_creator.c` that:

```c
/**
 * Creates N zombie processes for testing
 * Parent does NOT call wait() - children become zombies
 * Returns: 0 on success, -1 on failure
 */
int create_zombies(int count);
```

**Requirements:**
- Fork N child processes
- Children exit immediately with different exit codes
- Parent does NOT wait for children (creates zombies)
- Print PID of each zombie created
- Parent stays alive to keep zombies around

**Example Output:**
```
Created zombie: PID 1234 (exit code 0)
Created zombie: PID 1235 (exit code 1)
Created zombie: PID 1236 (exit code 2)
Press Enter to exit and clean up zombies...
```

**Verification:**
```bash
# Run program
./zombie_creator 5

# In another terminal, check for zombies
ps aux | grep 'Z'
# Should show 5 zombie processes
```

---

### Part 2: Zombie Detector (25 points)

Implement `zombie_detector.c` that scans the system for zombies:

```c
/**
 * Scans /proc filesystem for zombie processes
 * Returns: number of zombies found
 */
int find_zombies(int *zombie_pids, int max_zombies);

/**
 * Prints information about a zombie process
 */
void print_zombie_info(int pid);
```

**Requirements:**
- Read /proc/[pid]/stat for all processes
- Parse state field (3rd field) - look for 'Z' state
- Get parent PID (PPID) of zombie
- Get command name of zombie
- Print formatted zombie report

**Output Format:**
```
=== Zombie Process Report ===
Total Zombies: 3

PID     PPID    Command         State   Time
-----   -----   -------------   -----   --------
1234    1000    defunct         Z       00:05:23
1235    1000    test_process    Z       00:02:15
1236    1000    child           Z       00:00:10

Parent Process Analysis:
  PID 1000 (parent_proc) has 3 zombie children
```

---

### Part 3: Automatic Zombie Reaper (30 points)

Implement `zombie_reaper.c` with multiple reaping strategies:

#### Strategy 1: Explicit Wait
```c
/**
 * Reaps all child processes explicitly
 */
void reap_explicit(void);
```

#### Strategy 2: SIGCHLD Handler
```c
/**
 * Sets up SIGCHLD handler to auto-reap
 */
void setup_auto_reaper(void);

void sigchld_handler(int sig);
```

#### Strategy 3: Ignore SIGCHLD
```c
/**
 * Uses signal(SIGCHLD, SIG_IGN) for automatic reaping
 */
void setup_ignore_reaper(void);
```

**Requirements:**
- Implement all 3 strategies
- Demonstrate each strategy works
- Handle race conditions properly
- Reap all zombies, none left behind

**Test Program:**
```c
int main(int argc, char *argv[]) {
    int strategy = atoi(argv[1]); // 1, 2, or 3
    
    // Create 10 children
    for (int i = 0; i < 10; i++) {
        if (fork() == 0) {
            sleep(rand() % 3);
            exit(i);
        }
    }
    
    // Use chosen strategy
    switch(strategy) {
        case 1: reap_explicit(); break;
        case 2: setup_auto_reaper(); break;
        case 3: setup_ignore_reaper(); break;
    }
    
    sleep(5);  // Wait for all children
    
    // Verify no zombies
    system("ps aux | grep Z | grep -v grep");
    
    return 0;
}
```

---

### Part 4: Long-Running Daemon (15 points)

Implement `process_daemon.c` that never creates zombies:

```c
/**
 * Daemonize process - become background daemon
 */
void daemonize(void);

/**
 * Spawn worker processes periodically
 */
void spawn_workers(void);
```

**Requirements:**
- Daemonize properly (fork twice, setsid, close fds)
- Spawn worker processes every 5 seconds
- Workers do some work and exit
- NEVER create zombies (use SIGCHLD handler)
- Log activity to /tmp/daemon.log
- Handle SIGTERM for graceful shutdown

**Test:**
```bash
# Start daemon
./process_daemon

# Check it's running
ps aux | grep process_daemon

# Monitor for 60 seconds - should never see zombies
watch -n 1 'ps aux | grep defunct'

# Stop daemon
killall process_daemon
```

---

### Part 5: Zombie Prevention Library (10 points)

Create a reusable library `libzombie.a`:

**Header `zombie.h`:**
```c
#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <sys/types.h>

// Initialize zombie prevention (call once at program start)
void zombie_init(void);

// Fork with automatic zombie prevention
pid_t zombie_safe_fork(void);

// Spawn command with zombie prevention
int zombie_safe_spawn(const char *command, char *args[]);

// Get zombie statistics
typedef struct {
    int zombies_created;
    int zombies_reaped;
    int zombies_active;
} zombie_stats_t;

void zombie_get_stats(zombie_stats_t *stats);

#endif
```

**Requirements:**
- Implement all functions in `zombie.c`
- Compile as static library: `ar rcs libzombie.a zombie.o`
- Provide example program using the library
- Thread-safe (use mutexes if needed)

---

## 📦 Deliverables

Your submission must include:

```
homework2/
├── src/
│   ├── zombie_creator.c        # Part 1
│   ├── zombie_detector.c       # Part 2
│   ├── zombie_reaper.c         # Part 3
│   ├── process_daemon.c        # Part 4
│   ├── zombie.c                # Part 5
│   └── zombie.h                # Part 5
├── tests/
│   ├── test_creator.sh
│   ├── test_detector.sh
│   ├── test_reaper.sh
│   └── test_daemon.sh
├── Makefile                    # Build everything
├── README.md                   # Documentation
└── DEMO.md                     # Screenshots/output of tests
```

---

## 📊 Grading Rubric

| Component | Points | Criteria |
|-----------|--------|----------|
| **Part 1: Zombie Creator** | 20 | |
| - Creates zombies correctly | 10 | Forks and doesn't wait |
| - Proper verification | 5 | Can verify with ps |
| - Clean exit | 5 | Handles signals |
| **Part 2: Zombie Detector** | 25 | |
| - Scans /proc correctly | 10 | Reads all processes |
| - Parses stat correctly | 8 | Gets PID, PPID, state |
| - Formatted output | 7 | Clear, readable report |
| **Part 3: Zombie Reaper** | 30 | |
| - Strategy 1 (explicit) | 8 | wait() in loop |
| - Strategy 2 (SIGCHLD) | 12 | Handler reaps properly |
| - Strategy 3 (SIG_IGN) | 8 | Uses signal ignore |
| - No race conditions | 2 | Thread-safe |
| **Part 4: Daemon** | 15 | |
| - Proper daemonization | 5 | Fork twice, setsid |
| - Spawns workers | 5 | Every 5 seconds |
| - Never creates zombies | 5 | No zombies ever |
| **Part 5: Library** | 10 | |
| - All functions implemented | 5 | Complete API |
| - Compiles as library | 3 | .a file |
| - Example program | 2 | Shows usage |
| **Code Quality** | 10 | |
| - Error handling | 3 | Checks all returns |
| - Comments | 3 | Clear documentation |
| - No memory leaks | 2 | Valgrind clean |
| - Code style | 2 | Consistent |
| **Documentation** | 10 | |
| - README complete | 5 | All sections |
| - Test scripts work | 3 | Easy to run |
| - DEMO clear | 2 | Shows output |

**Total: 100 points**

---

## 🧪 Testing Requirements

### Test 1: Create and Verify Zombies
```bash
./zombie_creator 10 &
CREATOR_PID=$!
sleep 2
./zombie_detector
# Should show 10 zombies
kill $CREATOR_PID
```

### Test 2: Reaping Strategies
```bash
for strategy in 1 2 3; do
    echo "Testing strategy $strategy"
    ./zombie_reaper $strategy
    # Verify no zombies remain
    ps aux | grep defunct
done
```

### Test 3: Daemon
```bash
./process_daemon
sleep 60
# Should never see zombies
ps aux | grep defunct
killall process_daemon
```

### Test 4: Library
```bash
gcc -o test_lib test_lib.c -L. -lzombie
./test_lib
# Should create/reap processes without zombies
```

---

## 💡 Hints

### Reading /proc/[pid]/stat

```c
FILE *fp = fopen("/proc/1234/stat", "r");
int pid, ppid;
char comm[256], state;

fscanf(fp, "%d %s %c %d", &pid, comm, &state, &ppid);

if (state == 'Z') {
    printf("Zombie found: PID %d\n", pid);
}
```

### SIGCHLD Handler Template

```c
void sigchld_handler(int sig) {
    int status;
    pid_t pid;
    
    // Reap all terminated children
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Log if needed (use write(), not printf!)
    }
}

// Install handler
signal(SIGCHLD, sigchld_handler);
```

### Daemonize Template

```c
void daemonize(void) {
    pid_t pid = fork();
    if (pid > 0) exit(0);  // Parent exits
    
    setsid();  // New session
    
    pid = fork();
    if (pid > 0) exit(0);  // First child exits
    
    // Second child is daemon
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
```

---

## 📚 Resources

- `man 2 fork`
- `man 2 wait`
- `man 2 waitpid`
- `man 2 signal`
- `man 5 proc`
- Linux Process States: https://man7.org/linux/man-pages/man5/proc.5.html

---

## ❓ FAQ

**Q: Why do zombies exist?**  
A: They preserve exit status until parent calls wait(). Parent needs to know how child exited.

**Q: Are zombies harmful?**  
A: They consume PID entries. Too many zombies can exhaust PID space.

**Q: Can I kill a zombie with SIGKILL?**  
A: No! Zombies are already dead. You must reap them with wait() or kill the parent.

**Q: What if parent dies before reaping?**  
A: Init process (PID 1) adopts and reaps orphaned zombies.

---

## 🚀 Bonus Challenges (+10 points)

1. **GUI Monitor** (+5 points): Create ncurses-based live zombie monitor
2. **System-wide Reaper** (+5 points): Tool that can reap zombies from any parent (tricky!)

---

**Good luck! Remember: A zombie is just a process waiting to tell its exit story!** 🧟‍♂️

