# Linux Systems Programming Course
## Professional Operating Systems & Kernel Development

**Course Duration:** 8-10 weeks  
**Level:** Advanced Undergraduate / Graduate  
**Prerequisites:** C Programming, Data Structures, Basic Linux knowledge

---

## 📋 Course Overview

This course provides hands-on experience with Linux system programming, kernel internals, process management, and advanced debugging techniques. Students will learn to write system-level code, understand kernel behavior, and develop tools for process analysis and management.

---

## 🎯 Learning Objectives

By the end of this course, students will be able to:
1. Write C programs using system calls directly
2. Analyze and debug system-level programs using professional tools
3. Understand process lifecycle, scheduling, and inter-process communication
4. Implement custom kernel modules
5. Profile and optimize system performance
6. Debug complex concurrency issues and deadlocks

---

## 📚 Course Modules

### **Module 1: System Calls Programming** (Weeks 1-2)
**Topics:**
- Understanding the kernel-user space boundary
- Direct system call usage in C
- File operations: open(), read(), write(), close()
- Process operations: fork(), exec(), wait()
- Memory operations: mmap(), brk()
- Signal handling: signal(), sigaction()

**Tools:**
- `strace` - System call tracer
- `ltrace` - Library call tracer
- `gdb` - GNU Debugger
- `valgrind` - Memory profiler

**Homework 1:** Basic Process Management in C (100 points)
- Implement process creation with fork/exec
- Process monitoring and control
- Signal handling for graceful shutdown
- Process tree visualization

---

### **Module 2: Process Analysis & Monitoring** (Weeks 3-4)
**Topics:**
- Process states and lifecycle
- Process control block (PCB)
- Context switching
- /proc filesystem deep dive
- Performance monitoring and profiling

**Tools:**
- `ps` - Process snapshot
- `top/htop` - Interactive monitoring
- `iotop` - I/O monitoring
- `pidstat` - Process statistics
- `pstree` - Process tree visualization

**Homework 2:** Zombie Process Handler (100 points)
- Create zombie processes intentionally
- Implement automatic zombie reaper
- Detect and report zombie processes
- Prevent zombie accumulation

---

### **Module 3: CPU Scheduling** (Weeks 5-6)
**Topics:**
- Scheduling algorithms (FIFO, SJF, STCF, RR, MLFQ, CFS)
- Preemptive vs non-preemptive scheduling
- Real-time scheduling policies
- Linux Completely Fair Scheduler (CFS)
- Performance metrics: turnaround time, response time, fairness

**Tools:**
- Custom C simulators
- Performance analysis tools
- Visualization with GUI

**Homework 3:** Scheduler Simulator with GUI (100 points)
- Implement 5+ scheduling algorithms (FIFO, SJF, RR, MLFQ, CFS)
- Visual Gantt chart timeline
- Performance metrics comparison
- Interactive GUI with parameter tuning

---

### **Module 4: Inter-Process Communication** (Weeks 7-8)
**Topics:**
- Pipes and FIFOs
- Message queues
- Shared memory
- Semaphores and mutexes
- Sockets (UNIX domain and network)
- Signals for IPC

**Tools:**
- `ipcs` - IPC facility status
- `lsof` - List open files
- `netstat/ss` - Network statistics
- `strace` for IPC debugging

**Homework 4:** Chat System using IPC (100 points)
- Multi-client chat server/client
- Choice of IPC: pipes, message queues, or sockets
- Synchronization with semaphores
- Broadcast and private messaging
- Handle disconnections gracefully

---

### **Module 5: Kernel Modules & Advanced Debugging** (Weeks 9-10)
**Topics:**
- Kernel architecture overview
- Loadable kernel modules (LKM)
- Character device drivers
- Kernel space vs user space
- Concurrency and race conditions
- Deadlock detection and prevention

**Tools:**
- `insmod/rmmod/lsmod` - Module management
- `dmesg` - Kernel messages
- `gdb` with kernel debugging
- `/proc` and `/sys` filesystems
- `perf` - Performance analysis

**Homework 5:** Custom Kernel Module (100 points)
- Create loadable kernel module (LKM)
- Implement character device driver
- Add /proc filesystem interface
- User-space test program
- Handle multiple concurrent accesses

---

## 📊 Grading Breakdown

| Component | Weight | Points |
|-----------|--------|--------|
| Homework 1: Basic Process Management in C | 15% | 100 points |
| Homework 2: Zombie Process Handler | 15% | 100 points |
| Homework 3: Scheduler Simulator with GUI | 25% | 100 points |
| Homework 4: Chat System using IPC | 20% | 100 points |
| Homework 5: Custom Kernel Module | 25% | 100 points |
| **Total** | **100%** | **500 points** |

---

## 📖 Required Reading

### Books:
1. **"Operating Systems: Three Easy Pieces"** - Remzi & Andrea Arpaci-Dusseau (FREE online)
2. **"The Linux Programming Interface"** - Michael Kerrisk
3. **"Linux Kernel Development"** - Robert Love

### Online Resources:
- Linux man pages (`man 2 syscall`)
- Kernel documentation: https://www.kernel.org/doc/
- OSTEP: http://pages.cs.wisc.edu/~remzi/OSTEP/

---

## 🛠️ Development Environment

**Required Tools:**
```bash
# Compiler and build tools
sudo apt install build-essential gcc gdb make

# Debugging and profiling
sudo apt install strace ltrace valgrind

# Monitoring tools
sudo apt install htop iotop sysstat

# Kernel development
sudo apt install linux-headers-$(uname -r)

# GUI development (for scheduler simulator)
sudo apt install libgtk-3-dev libcairo2-dev
```

---

## 📝 Submission Guidelines

All homework must include:
1. **Source code** with extensive comments
2. **Makefile** for easy compilation
3. **README.md** with:
   - How to compile and run
   - Design decisions
   - Known issues/limitations
   - Sample output
4. **Test cases** and results
5. **Documentation** of any external libraries used

---

## 🎓 Academic Integrity

- You may discuss concepts with peers
- Code must be your own work
- Cite any external resources used
- Plagiarism will result in course failure

---

## 📧 Office Hours & Support

- Office Hours: TBD
- Discussion Forum: [Link to forum]
- Email response time: 24-48 hours

---

## 🚀 Course Progression Plan

We'll build the course materials in this order:
1. ✅ **Module 1:** System Calls - Lecture notes + examples + homework guide
2. **Module 2:** Process Analysis - Lecture notes + examples + homework guide
3. **Module 3:** Scheduling - Lecture notes + examples + homework guide
4. **Module 4:** IPC - Lecture notes + examples + homework guide
5. **Module 5:** Kernel Modules - Lecture notes + examples + homework guide

Each module will include:
- Detailed lecture notes (theory)
- Multiple C code examples with explanations
- Step-by-step homework guide
- Grading rubric

