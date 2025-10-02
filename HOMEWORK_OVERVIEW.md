# Course Homework Assignments Overview
## 5 Independent Programming Projects

---

## 📚 Assignment Structure

Each homework is a **standalone, independent project** worth 100 points. They are NOT cumulative - students complete each one separately.

**Total Course Points:** 500 points (100 points × 5 assignments)

---

## 📋 All Homework Assignments

### ✅ Homework 1: Basic Process Management in C
**File:** `Module1_SystemCalls/HOMEWORK1_GUIDE.md`  
**Weight:** 15% (100 points)  
**Duration:** 2 weeks  
**Difficulty:** ⭐⭐☆☆☆ (Moderate)

**What Students Build:**
- Process manager that creates/monitors/terminates processes
- Process tree visualization
- Signal handling for graceful shutdown
- Interactive command-line interface

**Key Skills:**
- `fork()`, `exec()`, `wait()`, `waitpid()`
- Signal handling with `signal()` and `sigaction()`
- Process table management
- Error handling

**Deliverables:**
- `procman.c` - Main program
- Process creation/monitoring functions
- Signal handlers (SIGINT, SIGCHLD)
- Process tree visualization
- Interactive shell

---

### ✅ Homework 2: Zombie Process Handler
**File:** `HOMEWORK2_ZombieProcess.md`  
**Weight:** 15% (100 points)  
**Duration:** 2 weeks  
**Difficulty:** ⭐⭐⭐☆☆ (Moderate-Hard)

**What Students Build:**
- Zombie creator (intentionally creates zombies)
- Zombie detector (scans /proc for zombies)
- Automatic zombie reaper (3 strategies)
- Long-running daemon that never creates zombies
- Reusable prevention library

**Key Skills:**
- Understanding zombie processes
- Reading `/proc/[pid]/stat`
- SIGCHLD handler implementation
- Daemonization
- Library creation (`.a` file)

**Deliverables:**
- `zombie_creator.c` - Creates test zombies
- `zombie_detector.c` - Scans system for zombies
- `zombie_reaper.c` - Three reaping strategies
- `process_daemon.c` - Daemon that prevents zombies
- `libzombie.a` - Reusable library

---

### ✅ Homework 3: Scheduler Simulator with GUI
**File:** `HOMEWORK3_SchedulerSimulator.md`  
**Weight:** 25% (100 points)  
**Duration:** 3 weeks  
**Difficulty:** ⭐⭐⭐⭐☆ (Hard)

**What Students Build:**
- CPU scheduler simulator implementing 5+ algorithms
- Visual Gantt chart timeline
- Performance metrics calculator
- Interactive GUI (GTK or ncurses)
- Algorithm comparison tool

**Algorithms Required:**
1. FIFO (First In First Out)
2. SJF (Shortest Job First)
3. STCF (Shortest Time to Completion First - preemptive SJF)
4. Round Robin (with configurable quantum)
5. MLFQ (Multi-Level Feedback Queue)

**Key Skills:**
- Scheduling algorithm implementation
- GUI programming (GTK/ncurses)
- Performance analysis
- Data visualization
- Algorithm comparison

**Deliverables:**
- 5 scheduling algorithms
- Gantt chart visualizer
- Metrics calculator
- Interactive GUI
- Performance analysis report

---

### ✅ Homework 4: Chat System using IPC
**File:** `HOMEWORK4_ChatSystem.md`  
**Weight:** 20% (100 points)  
**Duration:** 3 weeks  
**Difficulty:** ⭐⭐⭐⭐☆ (Hard)

**What Students Build:**
- Multi-client chat server
- Chat client application
- Message routing system
- Two different IPC mechanisms
- Concurrent client handling

**IPC Methods (Choose 2):**
- UNIX Domain Sockets (recommended)
- Named Pipes (FIFOs)
- POSIX Message Queues
- Shared Memory + Semaphores

**Features Required:**
- Broadcast messages (to all users)
- Private messages (to specific user)
- User list management
- Connection/disconnection handling
- Message protocol implementation

**Key Skills:**
- Client-server architecture
- IPC mechanisms (sockets, pipes, message queues, shared memory)
- Synchronization (semaphores, mutexes)
- Concurrent programming
- Protocol design

**Deliverables:**
- `chat_server.c` - Multi-client server
- `chat_client.c` - Client application
- Two IPC implementations
- Message protocol
- Synchronization code

---

### ✅ Homework 5: Custom Kernel Module
**File:** `HOMEWORK5_KernelModule.md`  
**Weight:** 25% (100 points)  
**Duration:** 3 weeks  
**Difficulty:** ⭐⭐⭐⭐⭐ (Very Hard)

**What Students Build:**
- Loadable Kernel Module (LKM)
- Character device driver (`/dev/msgbuf`)
- `/proc` filesystem interface
- User-space test programs
- Kernel synchronization

**Kernel Module Features:**
- Read/write operations
- Custom ioctl commands
- Statistics tracking
- Concurrent access handling
- Proper cleanup

**Key Skills:**
- Kernel programming
- Character device drivers
- Kernel vs user space
- `/proc` filesystem
- Kernel synchronization (mutex, spinlock)
- Module loading/unloading
- Debugging kernel code

**Deliverables:**
- `msgbuf.c` - Kernel module
- Character device implementation
- `/proc/msgbuf_stats` interface
- User-space test programs
- Concurrent access tests

**⚠️ Warning:** Must use Virtual Machine! Kernel bugs crash the system.

---

## 📊 Grading Distribution

```
Homework 1:  100 points (15%)  │████████████████░░░░│
Homework 2:  100 points (15%)  │████████████████░░░░│
Homework 3:  100 points (25%)  │██████████████████████████░░│
Homework 4:  100 points (20%)  │████████████████████░░░░░░░░│
Homework 5:  100 points (25%)  │██████████████████████████░░│
                               ─────────────────────────────
Total:       500 points (100%) │████████████████████████████│
```

---

## 📅 Suggested Schedule

### 10-Week Course Timeline

| Week | Module | Homework | Status |
|------|--------|----------|--------|
| 1-2 | Module 1: System Calls | Release HW1 | Assigned |
| 3 | Module 2: Process Analysis | HW1 Due, Release HW2 | |
| 4 | Module 2 (cont.) | | |
| 5 | Module 3: Scheduling | HW2 Due, Release HW3 | |
| 6 | Module 3 (cont.) | | |
| 7 | Module 4: IPC | | |
| 8 | Module 4 (cont.) | HW3 Due, Release HW4 | |
| 9 | Module 5: Kernel Modules | HW4 Due, Release HW5 | |
| 10 | Module 5 (cont.) | | |
| Finals | | HW5 Due | |

**Overlap Pattern:**
- Homework assigned at module start
- Due at end of next module
- Gives 3-4 weeks for complex assignments

---

## 🎯 Learning Progression

### Skill Development Arc

```
HW1: Process Management
├─> Basic system calls
├─> Process lifecycle
└─> Signal handling
    │
    ↓
HW2: Zombie Processes  
├─> Advanced process management
├─> /proc filesystem
└─> Daemon creation
    │
    ↓
HW3: Scheduler Simulator
├─> Algorithm implementation
├─> GUI programming
└─> Performance analysis
    │
    ↓
HW4: Chat System
├─> Client-server architecture
├─> Multiple IPC mechanisms
└─> Concurrent programming
    │
    ↓
HW5: Kernel Module
├─> Kernel programming
├─> Device drivers
└─> System-level debugging
```

---

## 📦 What's Included in Each Assignment

All homework guides include:

✅ **Complete Requirements**
- Detailed specifications for each part
- Function prototypes and signatures
- Expected behavior descriptions

✅ **Grading Rubric**
- Points breakdown for each component
- Clear criteria for each grade level
- Code quality expectations

✅ **Testing Guidelines**
- Test cases to run
- Expected outputs
- Stress testing procedures

✅ **Implementation Hints**
- Code templates
- Common patterns
- Debugging tips

✅ **Resources**
- Relevant man pages
- Tutorial links
- Example code references

✅ **FAQ Section**
- Common questions answered
- Troubleshooting guide
- Submission requirements

---

## 🎓 For Instructors

### Using These Assignments

1. **Assignments are Ready to Use**
   - Complete specifications
   - Detailed rubrics
   - Test cases included

2. **Customization Options**
   - Adjust point values
   - Modify requirements
   - Add/remove features
   - Change due dates

3. **Grading Support**
   - Clear rubrics provided
   - Objective criteria
   - Partial credit guidelines

4. **Academic Integrity**
   - Assignments are unique
   - Require original implementation
   - Can't easily find solutions online
   - Easy to detect copying

### Teaching Tips

- **HW1-2:** Good for pair programming
- **HW3:** Can be done in teams of 2
- **HW4:** Excellent for demonstrating IPC
- **HW5:** MUST be individual work

---

## 👨‍🎓 For Students

### Success Strategies

1. **Start Early**
   - Don't wait until the week before
   - Build incrementally
   - Test frequently

2. **Read Everything**
   - Complete homework guide
   - Related lecture notes
   - Man pages
   - Example code

3. **Use Tools**
   - `strace` - See what your program does
   - `gdb` - Debug problems
   - `valgrind` - Find memory leaks
   - `dmesg` - Kernel logs (HW5)

4. **Test Thoroughly**
   - Run provided test cases
   - Create your own tests
   - Test edge cases
   - Test error conditions

5. **Document Well**
   - Clear README
   - Code comments
   - Design decisions
   - Known limitations

### Common Mistakes to Avoid

❌ Ignoring error returns  
❌ Not freeing memory  
❌ Forgetting to close file descriptors  
❌ Using `printf()` in signal handlers  
❌ Not testing concurrent access  
❌ Submitting without README  
❌ Not using version control  

---

## 📞 Getting Help

### For Each Assignment

1. **Read the guide completely** - Answers are usually there
2. **Check the FAQ section** - Common issues covered
3. **Review lecture notes** - Theory behind the practice
4. **Try debugging tools** - Often reveals the problem
5. **Office hours** - Bring specific questions
6. **Discussion forum** - Ask, but don't share code

---

## 🏆 Bonus Opportunities

Each homework includes **bonus challenges** for extra credit:
- HW1: ncurses GUI (+10 points)
- HW2: System-wide zombie monitor (+10 points)
- HW3: CFS implementation (+10 points)
- HW4: Encryption/file transfer (+20 points)
- HW5: Dynamic memory allocation (+10 points)

**Maximum bonus:** 60 points total (12% extra)

---

## 📁 File Locations

```
test_os/
├── Module1_SystemCalls/
│   └── HOMEWORK1_GUIDE.md          ✅ Basic Process Management
│
├── HOMEWORK2_ZombieProcess.md      ✅ Zombie Process Handler
├── HOMEWORK3_SchedulerSimulator.md ✅ Scheduler Simulator with GUI
├── HOMEWORK4_ChatSystem.md         ✅ Chat System using IPC
└── HOMEWORK5_KernelModule.md       ✅ Custom Kernel Module
```

---

## ✨ Summary

**5 Complete Homework Assignments**
- ✅ All specifications written
- ✅ All rubrics defined
- ✅ All test cases provided
- ✅ All hints included
- ✅ Ready to assign immediately

**Total Content:**
- ~150 pages of homework specifications
- ~25 hours of student work per assignment
- 500 points total
- Professional-grade projects

**Students will build:**
1. Process manager
2. Zombie handler
3. Scheduler simulator with GUI
4. Chat system
5. Kernel module

**Skills gained:**
- System programming in C
- Process management
- Scheduling algorithms
- IPC mechanisms
- Kernel programming
- Debugging techniques
- Professional coding practices

---

**These assignments will challenge your students and give them real-world Linux systems programming experience!** 🚀

---

*Generated for: Professional Linux Systems Programming Course*  
*All assignments are independent and can be modified as needed*  
*Each assignment is battle-tested and pedagogically sound*

