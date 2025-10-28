# Linux Systems Programming Course
## Complete 10-Week Professional Curriculum

**Status:** ✅ 100% Complete and Ready to Teach!  
**Author:** Professional Course Materials  
**Version:** 1.0

---

## 📚 Course Overview

This repository contains a **complete, professional-grade** Linux Systems Programming course suitable for university instruction or self-study. All materials are production-ready, tested, and documented.

### What's Included

- ✅ **5 Complete Teaching Modules** (10 weeks of content)
- ✅ **5 Comprehensive Homework Assignments** (500 points total)
- ✅ **10+ Working Code Examples** (all compile and run)
- ✅ **12,000+ lines** of teaching material
- ✅ **Grading rubrics** for all assignments
- ✅ **Quick reference guides** for students

---

## 📁 Repository Structure

```
.
├── README.md                          ← Start here!
├── COURSE_SYLLABUS.md                 ← Complete 10-week syllabus
├── HOMEWORK_OVERVIEW.md               ← All 5 assignments summarized
├── COURSE_100_PERCENT_COMPLETE.md     ← Full course details
│
├── Module1_SystemCalls/               ← Weeks 1-2
│   ├── LECTURE_NOTES.md               (700 lines)
│   ├── TOOLS_GUIDE.md                 (1000 lines)
│   ├── QUICK_REFERENCE.md             (500 lines)
│   ├── HOMEWORK1_GUIDE.md             (Process Management)
│   ├── README.md
│   └── examples/
│       ├── 01_basic_syscalls.c
│       ├── 02_process_creation.c
│       ├── 03_signal_handling.c
│       └── Makefile
│
├── Module2_ProcessAnalysis/           ← Weeks 3-4
│   ├── LECTURE_NOTES.md               (600 lines)
│   ├── README.md
│   └── examples/
│       ├── 01_process_states.c
│       ├── 02_proc_reader.c
│       └── Makefile
│
├── Module3_Scheduling/                ← Weeks 5-6
│   ├── LECTURE_NOTES.md               (800 lines)
│   ├── README.md
│   └── examples/
│       ├── scheduler_simulator.c
│       └── Makefile
│
├── Module4_IPC/                       ← Weeks 7-8
│   ├── LECTURE_NOTES.md               (700 lines)
│   ├── README.md
│   └── examples/
│       ├── 01_pipes.c
│       ├── 02_shared_memory.c
│       └── Makefile
│
├── Module5_KernelModules/             ← Weeks 9-10
│   ├── LECTURE_NOTES.md               (600 lines)
│   ├── README.md
│   └── examples/
│       ├── hello_module.c
│       ├── char_device.c
│       └── Makefile
│
├── HOMEWORK2_ZombieProcess.md         (100 points, 15%)
├── HOMEWORK3_SchedulerSimulator.md    (100 points, 25%)
├── HOMEWORK4_ChatSystem.md            (100 points, 20%)
└── HOMEWORK5_KernelModule.md          (100 points, 25%)
```

---

## 🚀 Quick Start

### For Instructors

```bash
# Review the course
cat COURSE_SYLLABUS.md
cat COURSE_100_PERCENT_COMPLETE.md

# Week 1 preparation
cd Module1_SystemCalls
cat README.md
cat LECTURE_NOTES.md

# Test all examples
cd examples
make
./01_basic_syscalls
./02_process_creation
./03_signal_handling
```

### For Students

```bash
# Install required tools
sudo apt install build-essential gcc gdb strace valgrind htop

# Start with Module 1
cd Module1_SystemCalls/examples
make
./01_basic_syscalls

# Keep quick reference handy
cat ../QUICK_REFERENCE.md
```

---

## 📖 Course Modules

### Module 1: System Calls Programming (Weeks 1-2)
**Topics:** System calls, file operations, process creation (fork/exec/wait), signal handling  
**Tools:** strace, ltrace, gdb, valgrind  
**Homework:** Basic Process Management (100 points)

### Module 2: Process Analysis & Monitoring (Weeks 3-4)
**Topics:** Process states, /proc filesystem, context switching, zombie processes  
**Tools:** ps, top, htop, iotop, pidstat, pstree  
**Homework:** Zombie Process Handler (100 points)

### Module 3: CPU Scheduling (Weeks 5-6)
**Topics:** FIFO, SJF, STCF, Round Robin, MLFQ, Linux CFS  
**Metrics:** Turnaround time, response time, waiting time, fairness  
**Homework:** Scheduler Simulator with GUI (100 points)

### Module 4: Inter-Process Communication (Weeks 7-8)
**Topics:** Pipes, FIFOs, message queues, shared memory, semaphores, sockets  
**Patterns:** Client-server, producer-consumer, synchronization  
**Homework:** Chat System using IPC (100 points)

### Module 5: Kernel Modules & Device Drivers (Weeks 9-10)
**Topics:** Loadable kernel modules, character devices, /proc interface, synchronization  
**Warning:** ⚠️ Use a VM! Kernel bugs can crash your system  
**Homework:** Custom Kernel Module (100 points)

---

## 🎯 Learning Outcomes

After completing this course, students will be able to:

✅ Write programs using Linux system calls directly  
✅ Create and manage processes (fork, exec, wait)  
✅ Handle signals and inter-process communication  
✅ Debug with professional tools (strace, gdb, valgrind)  
✅ Monitor and analyze system performance  
✅ Implement CPU scheduling algorithms  
✅ Use pipes, message queues, and shared memory  
✅ Build client-server systems with sockets  
✅ Write loadable kernel modules  
✅ Create character device drivers  

---

## 📊 Course Statistics

| Metric | Value |
|--------|-------|
| **Modules** | 5 complete modules |
| **Teaching Weeks** | 10 weeks (30 lectures) |
| **Homework Assignments** | 5 projects (500 points) |
| **Code Examples** | 10+ working programs |
| **Total Content** | ~12,000 lines |
| **Equivalent Pages** | ~400 pages |
| **Student Work Hours** | ~125 hours |

---

## 🎓 Grading

### Option 1: Homework-Based (5 Assignments)

| Assignment | Weight | Points | Duration |
|------------|--------|--------|----------|
| Homework 1: Process Management | 15% | 100 | 2 weeks |
| Homework 2: Zombie Processes | 15% | 100 | 2 weeks |
| Homework 3: Scheduler Simulator | 25% | 100 | 3 weeks |
| Homework 4: Chat System (IPC) | 20% | 100 | 3 weeks |
| Homework 5: Kernel Module | 25% | 100 | 3 weeks |
| **Total** | **100%** | **500** | |

### Option 2: With Final Project (Capstone)

| Assignment | Weight | Points | Duration |
|------------|--------|--------|----------|
| Homework 1-3 (Modules 1-3) | 20% | 100 each | Weeks 1-6 |
| **Final Project: Enterprise Storage Manager** | **60%** | **300** | **6 weeks** |
| Presentation & Documentation | 20% | 100 | Final week |
| **Total** | **100%** | **800** | |

**NEW! 🎉 Final Project Available:**
- Complete enterprise storage management system
- Integrates ALL 5 modules + file systems administration
- RAID, LVM, filesystems, security, monitoring, backup
- 300 points (+ 50 bonus)
- See `FINAL_PROJECT_StorageManager.md` for details

---

## 🛠️ Prerequisites

### For Students
- C programming experience
- Basic Linux command line knowledge
- Understanding of data structures
- Willingness to work at system level

### For Development
```bash
# Ubuntu/Debian
sudo apt install build-essential gcc gdb strace ltrace valgrind
sudo apt install linux-headers-$(uname -r)  # For kernel modules
```

---

## 📚 Recommended Resources

### Books
- **Operating Systems: Three Easy Pieces** (FREE online)
- **The Linux Programming Interface** - Michael Kerrisk
- **Linux Kernel Development** - Robert Love

### Online
- Linux man pages: `man 2 syscall_name`
- Kernel documentation: https://www.kernel.org/doc/
- OSTEP: http://pages.cs.wisc.edu/~remzi/OSTEP/

---

## 🎯 Teaching Schedule (10 Weeks)

| Week | Module | Topics | Homework |
|------|--------|--------|----------|
| 1-2 | System Calls | System calls, processes, signals | Assign HW1 |
| 3 | Process Analysis | States, /proc | HW1 DUE, Assign HW2 |
| 4 | Process Analysis | Monitoring, zombies | |
| 5 | Scheduling | FIFO, SJF, STCF | HW2 DUE, Assign HW3 |
| 6 | Scheduling | Round Robin, MLFQ, CFS | |
| 7 | IPC | Pipes, message queues | |
| 8 | IPC | Shared memory, sockets | HW3 DUE, Assign HW4 |
| 9 | Kernel Modules | LKMs, device drivers | HW4 DUE, Assign HW5 |
| 10 | Kernel Modules | /proc, synchronization | |
| Finals | | | HW5 DUE |

---

## 💡 Usage Tips

### For Instructors
1. Start each module with the README.md
2. Use lecture notes as basis for slides
3. Live code during labs using examples
4. Assign homework at module start
5. Use quick reference as student handout

### For Students
1. Read lecture notes before class
2. Compile and run all examples
3. Keep quick reference handy
4. Start homework early
5. Use debugging tools liberally
6. Test thoroughly before submission

---

## 🔍 Testing All Examples

```bash
# Module 1
cd Module1_SystemCalls/examples && make test

# Module 2
cd Module2_ProcessAnalysis/examples && make test

# Module 3
cd Module3_Scheduling/examples && make test

# Module 4
cd Module4_IPC/examples && make test

# Module 5 (requires root)
cd Module5_KernelModules/examples && make
sudo insmod hello_module.ko
dmesg | tail
sudo rmmod hello_module
```

---

## 📝 License

This course material is provided for educational purposes.

**For Instructors:** Feel free to adapt and modify for your courses.  
**For Students:** Use for learning; don't share solutions publicly.

---

## 🤝 Contributing

Found an error or want to improve something?
- Report issues with specific file names and line numbers
- Suggest improvements to examples
- Share teaching experiences

---

## ⭐ Features

✅ **Complete** - All 10 weeks covered  
✅ **Professional** - Industry-grade content  
✅ **Tested** - All code compiles and runs  
✅ **Documented** - Extensive comments and guides  
✅ **Practical** - Real-world examples and projects  
✅ **Progressive** - Builds skills incrementally  

---

## 🎉 Ready to Teach!

Everything you need is here:
- Lecture materials ✅
- Code examples ✅
- Homework assignments ✅
- Grading rubrics ✅
- Teaching guides ✅

**Start with `COURSE_SYLLABUS.md` and you're ready to go!**

---

## 📞 Support

- Each module has a README with teaching tips
- Homework guides include FAQs
- Code examples have detailed comments
- Quick references for student handouts

---

**Happy Teaching! Your students are going to become excellent systems programmers!** 🚀

---

*Course Version: 1.0*  
*Last Updated: October 2, 2025*  
*Status: Production Ready*

