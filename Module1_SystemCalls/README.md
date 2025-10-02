# Module 1: System Calls Programming
## Complete Teaching Materials

---

## 📚 Module Contents

This module contains everything needed to teach **System Calls Programming** at a professional level.

### Files Included:

```
Module1_SystemCalls/
├── README.md                    ← You are here
├── LECTURE_NOTES.md            ← Complete theory and explanations
├── TOOLS_GUIDE.md              ← strace, ltrace, gdb, valgrind
├── QUICK_REFERENCE.md          ← Cheat sheet for students
├── HOMEWORK1_GUIDE.md          ← Assignment with rubric
└── examples/
    ├── Makefile                ← Build all examples
    ├── 01_basic_syscalls.c     ← File operations example
    ├── 02_process_creation.c   ← fork/exec/wait example
    └── 03_signal_handling.c    ← Signal handling example
```

---

## 🎯 Learning Path

### Week 1: Introduction and File Operations

#### Day 1-2: Introduction
- **Lecture**: Sections 1-3 of LECTURE_NOTES.md
  - What are system calls?
  - Kernel vs User space
  - How system calls work
- **Demo**: Run `strace ls` to show system calls in action
- **Lab**: Compile and run `01_basic_syscalls.c`

#### Day 3-4: File Operations
- **Lecture**: Section 4 of LECTURE_NOTES.md (File Operations)
- **Demo**: Walk through `01_basic_syscalls.c` line by line
- **Lab**: Students modify example to:
  - Copy files
  - Count lines in a file
  - Implement `cat` command
- **Tool Practice**: Use `strace -e trace=file ./program`

#### Day 5: Error Handling
- **Lecture**: Section 5 of LECTURE_NOTES.md (Error Handling)
- **Demo**: Intentionally cause errors and handle them
- **Lab**: Add comprehensive error handling to previous labs

---

### Week 2: Process Management and Signals

#### Day 1-2: Process Creation
- **Lecture**: Process operations from LECTURE_NOTES.md
- **Demo**: Walk through `02_process_creation.c`
- **Lab**: Students implement:
  - Create N child processes
  - Process chain (P1→P2→P3)
  - Simple shell that can run commands
- **Tool Practice**: `strace -f` to follow forks

#### Day 3-4: Signal Handling
- **Lecture**: Signal operations from LECTURE_NOTES.md
- **Demo**: Walk through `03_signal_handling.c`
- **Lab**: Students implement:
  - Ctrl+C handler
  - Custom signal communication
  - Timeout using SIGALRM
- **Tool Practice**: Sending signals with `kill` command

#### Day 5: Debugging and Tools
- **Lecture**: TOOLS_GUIDE.md
- **Demo**: Live debugging session with gdb
- **Lab**: Debug programs with memory leaks using valgrind
- **Assignment**: Release HOMEWORK1_GUIDE.md

---

## 🏫 Teaching Tips

### For Instructors:

1. **Start with strace**
   - Show `strace ls` before writing any code
   - This demystifies system calls immediately
   - Students can see what "really happens"

2. **Live Coding**
   - Write code during class, making mistakes
   - Show how to debug errors in real-time
   - Use gdb to step through code together

3. **Interactive Demos**
   - Terminal 1: Run program
   - Terminal 2: Send signals, check `/proc`, use `ps`
   - Terminal 3: Show `strace` or `lsof` output

4. **Common Pitfalls**
   - Forgetting to check return values → Show crash
   - Not waiting for children → Show zombie processes
   - Using `printf()` in signal handlers → Explain why unsafe

5. **Real-World Examples**
   - "How does `ssh` work?" → fork + exec
   - "How does a web server handle requests?" → fork/threads
   - "Why do programs hang?" → waiting for I/O

---

## 🧪 Lab Exercises

### Quick Exercises (In-Class)

#### Exercise 1: File Operations (15 mins)
```c
// Task: Write a program that counts characters in a file
// Use: open(), read(), close()
// Bonus: Count lines and words too
```

#### Exercise 2: Process Creation (20 mins)
```c
// Task: Create 3 children, each prints their PID
// Use: fork(), getpid(), wait()
// Bonus: Make them execute different programs
```

#### Exercise 3: Signal Handling (20 mins)
```c
// Task: Program that runs until SIGUSR1, then exits gracefully
// Use: signal(), pause()
// Test: kill -SIGUSR1 <pid>
```

### Extended Lab Projects

#### Lab 1: File Manager (2 hours)
Build a program that can:
- List files in directory
- Copy files
- Delete files
- Show file info
Only use system calls (no `fopen`, `fread`, etc.)

#### Lab 2: Process Monitor (2 hours)
Build a program that:
- Launches child processes
- Monitors their status
- Kills them on timeout
- Shows process tree

#### Lab 3: Signal-Based IPC (2 hours)
Build two programs that communicate via signals:
- Sender sends SIGUSR1/SIGUSR2
- Receiver interprets as binary (0/1)
- Transfer a byte using 8 signals

---

## 📊 Assessment Ideas

### Quizzes (10-15 mins)

**Quiz 1: System Call Basics**
1. What's the difference between `fork()` return values in parent vs child?
2. What happens if you don't close file descriptors?
3. Name 3 signals that can't be caught.
4. What does `errno` contain after a successful system call?

**Quiz 2: Debugging Tools**
1. Which tool traces system calls?
2. How do you follow forks with strace?
3. What does valgrind detect?
4. How do you attach gdb to a running process?

### Practical Exams (30-45 mins)

**Exam 1: Implement `cat` command**
- Takes filename as argument
- Reads and prints to stdout
- Handles errors properly
- Uses only system calls

**Exam 2: Process launcher**
- Forks and execs a command
- Waits for completion
- Prints exit status
- Handles signals

---

## 🎓 Homework 1 Grading

See `HOMEWORK1_GUIDE.md` for complete assignment.

### Grading Workflow:

1. **Compilation (5 mins)**
   ```bash
   cd homework1/
   make
   # Should compile without warnings
   ```

2. **Functionality Testing (15 mins)**
   ```bash
   make test
   # Run provided test scripts
   ./test_scripts/test1.sh
   ./test_scripts/test2.sh
   ./test_scripts/test3.sh
   ```

3. **Memory Check (5 mins)**
   ```bash
   make valgrind
   # Should show "no leaks are possible"
   ```

4. **Code Review (15 mins)**
   - Check error handling
   - Check comments
   - Check code style
   - Look for race conditions

5. **Interactive Testing (10 mins)**
   - Run program manually
   - Try edge cases
   - Test signal handling
   - Check zombie prevention

### Common Issues:

| Issue | Points Deducted | Fix |
|-------|-----------------|-----|
| Doesn't compile | -20 | Check Makefile |
| No error handling | -10 | Check all returns |
| Memory leaks | -5 | Run valgrind |
| Zombie processes | -10 | Handle SIGCHLD |
| No comments | -5 | Add documentation |
| Segmentation fault | -15 | Test thoroughly |

---

## 📖 Additional Resources

### Books
- **"The Linux Programming Interface"** by Michael Kerrisk
  - Chapters 3-6: System calls
  - Chapters 24-28: Processes
  - Chapters 20-22: Signals

- **"Operating Systems: Three Easy Pieces"**
  - Chapter 5: Process API
  - Free online: http://pages.cs.wisc.edu/~remzi/OSTEP/

### Online
- Linux man pages: https://man7.org/
- Syscall reference: https://syscalls.kernelgrok.com/
- strace examples: https://strace.io/
- GNU Debugger: https://sourceware.org/gdb/documentation/

### Videos
- "System Calls" - MIT 6.004
- "Process Management" - UC Berkeley CS162
- "Debugging with GDB" - Tutorial by Chris Terman

---

## 💻 Setting Up Environment

### Required Software

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y \
    build-essential \
    gcc \
    gdb \
    strace \
    ltrace \
    valgrind \
    man-db \
    manpages-dev

# Verify installation
gcc --version
gdb --version
strace --version
valgrind --version
```

### Recommended VS Code Extensions
- C/C++ (Microsoft)
- C/C++ Extension Pack
- Native Debug
- Makefile Tools

### Recommended Terminal Setup
- Use multiple terminal panes
- Install `tmux` for terminal multiplexing
- Set up aliases:
  ```bash
  alias gccw='gcc -Wall -Wextra -g -std=c11'
  alias vg='valgrind --leak-check=full'
  alias st='strace -e trace=file,process'
  ```

---

## 🐛 Troubleshooting

### "Permission denied" when running strace
```bash
# Solution 1: Run with sudo
sudo strace ./program

# Solution 2: Change ptrace_scope (Ubuntu)
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
```

### "No debugging symbols found"
```bash
# Recompile with -g flag
gcc -g -o program program.c
```

### "Child process is a zombie"
```c
// Add SIGCHLD handler
signal(SIGCHLD, SIG_IGN);
// Or
void handler(int sig) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
signal(SIGCHLD, handler);
```

### "Too many open files"
```bash
# Check limit
ulimit -n

# Increase limit
ulimit -n 4096

# Or close file descriptors when done
close(fd);
```

---

## 🤝 Contributing

If you're adapting this module:

### Easy Modifications:
- Change homework requirements to match your course
- Add/remove examples as needed
- Adjust difficulty for your students
- Add more quiz questions

### Please Keep:
- Emphasis on error handling
- Use of debugging tools
- Real system calls (not library wrappers)
- Valgrind memory checking

### Feedback Welcome:
- What worked well?
- What was too difficult?
- What examples would help?
- Any errors found?

---

## 📧 Support

For questions about this module:
- Check LECTURE_NOTES.md first
- Try QUICK_REFERENCE.md for syntax
- See TOOLS_GUIDE.md for debugging
- Review examples/ directory

---

## ✅ Checklist for Instructors

Before teaching this module:

- [ ] Review all lecture notes
- [ ] Compile and test all examples
- [ ] Run through homework assignment yourself
- [ ] Prepare live coding examples
- [ ] Set up demo environment with multiple terminals
- [ ] Test debugging tools (strace, gdb, valgrind)
- [ ] Prepare answers to common questions
- [ ] Create grading rubric spreadsheet
- [ ] Schedule office hours for homework help

During the module:

- [ ] Start each class with review of previous material
- [ ] Live code at least one example per class
- [ ] Show real-world use cases
- [ ] Encourage questions
- [ ] Assign practice exercises
- [ ] Monitor student progress
- [ ] Adjust pace as needed

After the module:

- [ ] Collect student feedback
- [ ] Grade homework consistently
- [ ] Provide detailed feedback
- [ ] Review common mistakes in next class
- [ ] Update materials based on feedback

---

**Good luck with your course! Your students are going to become system programming experts!** 🚀

