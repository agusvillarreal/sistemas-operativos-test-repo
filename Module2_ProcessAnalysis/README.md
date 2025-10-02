# Module 2: Process Analysis & Monitoring

Complete teaching materials for understanding process states, the /proc filesystem, and monitoring tools.

## 📁 Contents

- `LECTURE_NOTES.md` - Complete theory and explanations
- `examples/` - Working C code demonstrations
  - `01_process_states.c` - All process states (R, S, D, Z, T)
  - `02_proc_reader.c` - Read /proc filesystem
- `Makefile` - Build all examples

## 🎯 Learning Objectives

Students will learn to:
1. Understand all Linux process states
2. Read and parse /proc filesystem
3. Monitor processes with ps, top, htop
4. Identify and handle zombie processes
5. Measure CPU, memory, and I/O usage

## 🚀 Quick Start

```bash
cd examples/
make
./01_process_states    # See all process states
./02_proc_reader list  # List all processes
```

## 📚 Teaching Guide

### Week 3: Process States and /proc

**Day 1-2:** Process States
- Lecture: LECTURE_NOTES.md sections 1-2
- Demo: `01_process_states.c`
- Lab: Students create processes in different states

**Day 3-4:** /proc Filesystem
- Lecture: LECTURE_NOTES.md section 3
- Demo: `02_proc_reader.c`
- Lab: Write tools to read /proc

**Day 5:** Zombie Processes
- Lecture: LECTURE_NOTES.md section 7
- Demo: Create and reap zombies
- Assign: **Homework 2** (Zombie Process Handler)

### Week 4: Monitoring Tools

**Day 1-2:** ps, top, htop
- Lecture: LECTURE_NOTES.md section 5
- Demo: Live monitoring session
- Lab: Process monitoring exercises

**Day 3-4:** Advanced Monitoring
- Tools: pidstat, pstree, iotop
- Performance metrics
- Context switching

**Day 5:** Review and Q&A
- Homework 2 help session
- Practice problems

## ✅ Ready to Use!

All materials are complete and tested.

