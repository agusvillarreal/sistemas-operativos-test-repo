# Module 2: Process Analysis & Monitoring
## Lecture Notes

---

## 📚 Table of Contents
1. [Process States and Lifecycle](#process-states)
2. [Process Control Block (PCB)](#pcb)
3. [The /proc Filesystem](#proc-filesystem)
4. [Context Switching](#context-switching)
5. [Process Monitoring Tools](#monitoring-tools)
6. [Performance Metrics](#performance-metrics)
7. [Zombie Processes Deep Dive](#zombie-processes)

---

## 1. Process States and Lifecycle {#process-states}

### Process State Model

In Linux, a process can be in one of several states:

```
     ┌──────────┐
     │  CREATED │
     └────┬─────┘
          │
          ↓
     ┌──────────┐     preempted    ┌──────────┐
  ┌─→│  READY   │←─────────────────│ RUNNING  │←─┐
  │  │ (Runnable)│                  │          │  │
  │  └────┬─────┘                   └────┬─────┘  │
  │       │                              │         │
  │       │ scheduled                    │         │
  │       └──────────────────────────────┘         │
  │                                                 │
  │                  I/O complete / event           │
  │              ┌────────────────────────┐         │
  └──────────────│   BLOCKED (Waiting)    │         │
                 │  - I/O wait            │         │
                 │  - Event wait          │         │
                 │  - Sleep               │         │
                 └────────────────────────┘         │
                            ↓                        │
                      I/O requested                  │
                            └────────────────────────┘
                            
                 ┌────────────────────────┐
                 │   ZOMBIE (Defunct)     │
                 │   Exit, awaiting       │
                 │   parent's wait()      │
                 └────────────────────────┘
```

### Linux Process State Codes

| Code | State | Description |
|------|-------|-------------|
| **R** | Running | Executing or ready to execute |
| **S** | Sleeping | Interruptible sleep (waiting for event) |
| **D** | Disk Sleep | Uninterruptible sleep (usually I/O) |
| **T** | Stopped | Stopped by job control signal |
| **t** | Tracing Stop | Stopped by debugger |
| **Z** | Zombie | Terminated but not reaped by parent |
| **X** | Dead | Should never be visible |
| **I** | Idle | Kernel thread (since 4.14) |

### Process State Transitions

#### 1. Creation → Ready
```c
pid_t pid = fork();
// New process created in READY state
// Added to scheduler's run queue
```

#### 2. Ready → Running
```
Scheduler selects process
CPU allocated to process
Process begins execution
```

#### 3. Running → Ready (Preemption)
```
Time slice expires
Higher priority process arrives
Voluntary yield (sched_yield())
```

#### 4. Running → Blocked
```c
// Process makes blocking system call
read(fd, buffer, size);      // Wait for I/O
sleep(5);                     // Wait for timer
wait(&status);                // Wait for child
sem_wait(&sem);               // Wait for semaphore
```

#### 5. Blocked → Ready
```
I/O completes
Timer expires
Child terminates
Semaphore posted
```

#### 6. Running → Zombie
```c
exit(0);        // Process terminates
// Becomes zombie until parent calls wait()
```

#### 7. Zombie → Dead
```c
// In parent:
wait(&status);  // Reaps zombie
// Process completely removed from system
```

---

## 2. Process Control Block (PCB) {#pcb}

### What is a PCB?

The **Process Control Block** (task_struct in Linux) is the kernel's data structure for each process.

### Key Fields in task_struct

```c
struct task_struct {
    // Process identification
    pid_t pid;                      // Process ID
    pid_t tgid;                     // Thread group ID
    
    // State
    volatile long state;            // Process state (R, S, D, etc.)
    int exit_state;                 // Exit state
    unsigned int flags;             // Process flags
    
    // Scheduling
    int prio;                       // Dynamic priority
    int static_prio;                // Static priority
    unsigned int policy;            // Scheduling policy
    struct sched_entity se;         // Scheduling entity
    
    // Memory management
    struct mm_struct *mm;           // Memory descriptor
    struct mm_struct *active_mm;    // Active memory
    
    // File system
    struct fs_struct *fs;           // Filesystem info
    struct files_struct *files;     // Open file descriptors
    
    // Parent/child relationships
    struct task_struct *parent;     // Parent process
    struct list_head children;      // List of children
    struct list_head sibling;       // Sibling list
    
    // Credentials
    const struct cred *cred;        // User/group IDs
    
    // Time accounting
    u64 utime;                      // User CPU time
    u64 stime;                      // System CPU time
    u64 start_time;                 // Process start time
    
    // Signal handling
    struct signal_struct *signal;   // Signal handlers
    sigset_t blocked;               // Blocked signals
    
    // Limits
    struct rlimit rlim[RLIM_NLIMITS];  // Resource limits
};
```

### Viewing Process Information

The kernel exposes process information through `/proc/[pid]/`:

```bash
# Process state
cat /proc/1234/stat

# Memory maps
cat /proc/1234/maps

# Open files
ls -l /proc/1234/fd/

# Command line
cat /proc/1234/cmdline

# Environment
cat /proc/1234/environ

# Status (human-readable)
cat /proc/1234/status
```

---

## 3. The /proc Filesystem {#proc-filesystem}

### What is /proc?

`/proc` is a **pseudo-filesystem** that provides an interface to kernel data structures. It doesn't contain "real" files but rather dynamically generated content.

### Important /proc Directories

```
/proc/
├── [pid]/              # Per-process information
│   ├── cmdline         # Command line arguments
│   ├── cwd             # Current working directory (symlink)
│   ├── environ         # Environment variables
│   ├── exe             # Executable (symlink)
│   ├── fd/             # Open file descriptors
│   ├── maps            # Memory mappings
│   ├── stat            # Process statistics
│   ├── status          # Human-readable status
│   ├── task/           # Threads
│   ├── io              # I/O statistics
│   └── limits          # Resource limits
├── cpuinfo             # CPU information
├── meminfo             # Memory information
├── loadavg             # System load averages
├── uptime              # System uptime
├── version             # Kernel version
└── sys/                # Kernel parameters
```

### Reading /proc/[pid]/stat

The `stat` file contains process statistics in a single line:

```c
// Format:
// pid (comm) state ppid pgrp session tty_nr tpgid flags minflt cminflt majflt...

char stat_path[256];
sprintf(stat_path, "/proc/%d/stat", pid);

FILE *fp = fopen(stat_path, "r");
int pid, ppid, pgrp;
char comm[256], state;
unsigned long utime, stime;

fscanf(fp, "%d %s %c %d %d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
       &pid, comm, &state, &ppid, &pgrp, &utime, &stime);
```

### Field Positions in /proc/[pid]/stat

| Field | Name | Description |
|-------|------|-------------|
| 1 | pid | Process ID |
| 2 | comm | Executable name (in parentheses) |
| 3 | state | State (R/S/D/Z/T) |
| 4 | ppid | Parent process ID |
| 5 | pgrp | Process group ID |
| 14 | utime | User CPU time (clock ticks) |
| 15 | stime | System CPU time (clock ticks) |
| 22 | starttime | Time process started (since boot) |
| 23 | vsize | Virtual memory size (bytes) |
| 24 | rss | Resident set size (pages) |

### Reading /proc/[pid]/status

More human-readable format:

```bash
$ cat /proc/1234/status
Name:   myprocess
State:  S (sleeping)
Tgid:   1234
Pid:    1234
PPid:   1000
TracerPid:  0
Uid:    1000    1000    1000    1000
Gid:    1000    1000    1000    1000
FDSize: 256
VmSize:     4096 kB
VmRSS:      1024 kB
Threads:    1
```

### Example: Reading Process Info

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    char comm[256];
    char state;
    int ppid;
    unsigned long utime;
    unsigned long stime;
} proc_info_t;

int read_proc_stat(int pid, proc_info_t *info) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    
    int ret = fscanf(fp, "%d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                     &info->pid, info->comm, &info->state, 
                     &info->ppid, &info->utime, &info->stime);
    
    fclose(fp);
    return (ret == 6) ? 0 : -1;
}
```

---

## 4. Context Switching {#context-switching}

### What is Context Switching?

**Context switching** is the process of saving the state of a running process and restoring the state of another process.

### What Gets Saved/Restored?

1. **CPU Registers**
   - General purpose registers (RAX, RBX, etc.)
   - Program counter (instruction pointer)
   - Stack pointer
   - Flags register

2. **Memory Context**
   - Page tables
   - Memory mappings

3. **Kernel State**
   - Open files
   - Signal handlers
   - Scheduling information

### Context Switch Steps

```
Running Process A
      ↓
1. Timer interrupt / system call
      ↓
2. Save Process A state to its PCB
   - CPU registers → PCB_A
   - Stack pointer → PCB_A
   - Program counter → PCB_A
      ↓
3. Scheduler selects Process B
      ↓
4. Restore Process B state from its PCB
   - PCB_B → CPU registers
   - PCB_B → Stack pointer
   - PCB_B → Program counter
      ↓
5. Switch memory context (page tables)
      ↓
Running Process B
```

### Cost of Context Switching

Context switches are **expensive**:

- **Direct costs**: 1-10 microseconds
  - Save/restore registers: ~100-500 ns
  - Switch page tables: ~1-5 µs
  - TLB flush: Can be expensive

- **Indirect costs**:
  - Cache pollution (cold cache)
  - TLB misses
  - Pipeline flush

### Measuring Context Switches

```bash
# Per-process context switches
cat /proc/1234/status | grep ctxt
voluntary_ctxt_switches:    1250
nonvoluntary_ctxt_switches: 45

# System-wide
vmstat 1
procs -----------memory---------- ---swap-- -----io---- -system-- ------cpu-----
 r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st
 1  0      0 123456  78901 234567    0    0     5    10  250 3500  5  2 93  0  0
                                                           ↑    ↑
                                                      interrupts context_switches
```

### Voluntary vs Involuntary Context Switches

**Voluntary:**
- Process blocks (I/O, sleep, wait)
- Process yields CPU (sched_yield)
- Process exits

**Involuntary:**
- Time slice expires
- Higher priority process preempts
- Interrupt handling

---

## 5. Process Monitoring Tools {#monitoring-tools}

### ps - Process Snapshot

```bash
# All processes
ps aux

# Process tree
ps auxf
ps -ejH

# Custom format
ps -eo pid,ppid,cmd,stat,%mem,%cpu

# Threads
ps -eLf
```

**Output Fields:**
```
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root         1  0.0  0.1 169416 11432 ?        Ss   Oct01   0:03 /sbin/init
```

**STAT Codes:**
- `R` - Running
- `S` - Sleeping
- `D` - Uninterruptible sleep
- `Z` - Zombie
- `T` - Stopped
- `+` - Foreground process group
- `<` - High priority
- `N` - Low priority
- `L` - Pages locked in memory
- `s` - Session leader
- `l` - Multi-threaded

### top - Interactive Monitor

```bash
top                    # Interactive
top -b -n 1            # Batch mode, one iteration
top -p 1234            # Monitor specific PID
top -u username        # Monitor user's processes
```

**Top Commands:**
- `h` - Help
- `k` - Kill process
- `r` - Renice process
- `M` - Sort by memory
- `P` - Sort by CPU
- `1` - Show all CPUs
- `f` - Select fields

### htop - Enhanced top

```bash
htop                   # Better UI
htop -p 1234,5678      # Monitor specific PIDs
htop -u username       # Filter by user
```

**Features:**
- Color-coded output
- Mouse support
- Tree view (F5)
- Process search (F3)
- Kill signals (F9)

### pidstat - Process Statistics

```bash
# CPU statistics, update every 1 second
pidstat 1

# Memory statistics
pidstat -r 1

# I/O statistics
pidstat -d 1

# Page faults
pidstat -R 1

# Context switches
pidstat -w 1

# Specific process
pidstat -p 1234 1
```

### pstree - Process Tree

```bash
# Full process tree
pstree

# Show PIDs
pstree -p

# Show threads
pstree -t

# Specific process
pstree -p 1234

# User's processes
pstree username
```

### iotop - I/O Monitor

```bash
# Monitor I/O
sudo iotop

# Batch mode
sudo iotop -b -n 1

# Processes doing I/O
sudo iotop -o

# Specific process
sudo iotop -p 1234
```

---

## 6. Performance Metrics {#performance-metrics}

### CPU Metrics

#### 1. CPU Utilization
```
CPU% = (utime + stime) / real_time × 100
```

#### 2. User vs System Time
```bash
time ./program
real    0m5.234s    # Wall clock time
user    0m3.456s    # User CPU time
sys     0m0.678s    # Kernel CPU time
```

#### 3. Load Average
```bash
uptime
 15:30:45 up 10 days, 2:34, 3 users, load average: 1.25, 0.98, 0.76
                                                    1min  5min  15min
```

**Interpretation:**
- Load < # CPUs: System not busy
- Load = # CPUs: System fully utilized
- Load > # CPUs: Processes waiting for CPU

### Memory Metrics

#### 1. Virtual Memory Size (VSZ)
- Total virtual memory allocated
- Includes code, data, libraries, stack

#### 2. Resident Set Size (RSS)
- Physical memory actually used
- Does not include swapped out pages

#### 3. Shared Memory
- Memory shared with other processes
- Libraries, shared segments

```bash
# Memory usage per process
ps aux | awk '{print $2, $6, $11}' | head
PID    RSS   COMMAND
1      11432 /sbin/init
123    45678 /usr/bin/myapp
```

### I/O Metrics

```bash
# I/O statistics
cat /proc/1234/io
rchar: 1234567890       # Bytes read
wchar: 987654321        # Bytes written
syscr: 12345            # Read syscalls
syscw: 6789             # Write syscalls
read_bytes: 1048576     # Actual disk reads
write_bytes: 524288     # Actual disk writes
```

---

## 7. Zombie Processes Deep Dive {#zombie-processes}

### What is a Zombie?

A **zombie** is a process that has:
- Completed execution (called `exit()`)
- But hasn't been reaped by its parent (parent hasn't called `wait()`)

### Why Do Zombies Exist?

**Purpose:** Preserve exit status for parent

```c
// Child process
exit(42);  // Exit with status 42

// Parent needs to retrieve this status:
int status;
wait(&status);
int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
// exit_code = 42
```

### Zombie State Diagram

```
Process Running
     ↓
exit() called
     ↓
┌─────────────────────┐
│  ZOMBIE STATE       │  <- Most resources freed
│                     │  <- Only PCB remains
│  - No memory        │  <- Occupies PID
│  - No CPU          │  <- Parent can read exit status
│  - No files        │
└─────────────────────┘
     ↓
Parent calls wait()
     ↓
Process Removed
```

### Identifying Zombies

```bash
# Find all zombies
ps aux | grep 'Z'

# Or more precisely
ps aux | awk '$8=="Z" {print $0}'

# Count zombies
ps aux | grep -c '^[^ ]* *[^ ]* *[^ ]* *[^ ]* *[^ ]* *[^ ]* *[^ ]* *Z'

# Show parent of zombies
ps -eo pid,ppid,stat,comm | grep Z
```

### Creating a Zombie (DON'T DO IN PRODUCTION!)

```c
#include <unistd.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child exits immediately
        exit(0);
    }
    
    // Parent doesn't call wait() - CREATES ZOMBIE
    sleep(60);  // Child is zombie for 60 seconds
    
    return 0;
}
```

### Preventing Zombies

#### Method 1: Explicit wait()
```c
int status;
wait(&status);  // or waitpid()
```

#### Method 2: SIGCHLD Handler
```c
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

signal(SIGCHLD, sigchld_handler);
```

#### Method 3: Ignore SIGCHLD
```c
signal(SIGCHLD, SIG_IGN);  // Kernel auto-reaps
```

#### Method 4: Double Fork
```c
if (fork() == 0) {
    if (fork() == 0) {
        // Grandchild does work
        // When done, init (PID 1) will reap it
    }
    exit(0);  // First child exits immediately
}
wait(NULL);  // Reap first child
```

### What Happens to Zombie's Parent?

If parent dies before reaping:
```
Zombie's parent dies
        ↓
Zombie becomes orphan
        ↓
init (PID 1) adopts zombie
        ↓
init reaps zombie
        ↓
Zombie removed
```

### Problems with Zombies

1. **PID Exhaustion**
   - Each zombie occupies a PID
   - System has limited PIDs (typically 32768)
   - Too many zombies = can't create new processes

2. **Process Table Pollution**
   - Zombies stay in process table
   - Wastes kernel memory (small amount per zombie)

3. **Symptoms of Parent Bug**
   - Indicates parent not properly managing children
   - May indicate resource leaks elsewhere

---

## 📝 Summary

### Key Takeaways

1. **Process States**: R, S, D, Z, T - understand transitions
2. **PCB**: Kernel's data structure for processes (task_struct)
3. **/proc**: Window into kernel - read process info
4. **Context Switches**: Expensive - minimize them
5. **Monitoring Tools**: ps, top, htop, pidstat, pstree, iotop
6. **Zombies**: Must be reaped - use wait() or SIGCHLD handler

### Common Patterns

```c
// Read process state
FILE *fp = fopen("/proc/PID/stat", "r");
fscanf(fp, "%d %s %c %d", &pid, comm, &state, &ppid);

// Prevent zombies
signal(SIGCHLD, SIG_IGN);  // or setup handler

// Monitor children
while (waitpid(-1, &status, WNOHANG) > 0) {
    // Child reaped
}
```

---

## 🔍 Additional Resources

- `man 5 proc` - /proc filesystem documentation
- `man ps` - Process snapshot
- `man top` - Interactive monitor
- `/proc/[pid]/` - Process information directory
- Linux kernel source: `include/linux/sched.h`

---

**Next Module:** CPU Scheduling Algorithms

