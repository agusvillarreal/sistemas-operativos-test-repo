# Simple Semaphore Implementation Demo

## 📚 What This Shows

This is a **simplified educational implementation** of the semaphore concept from your slides!

## The Code Structure

### 1. The Semaphore Structure
```c
struct semaphore {
    int count;  // number of available resources
    // In real OS: queue q;  // waiting processes
};
```

### 2. The Wait Operation (P operation)
```c
void sem_wait(struct semaphore *s) {
    s->count--;  // ALWAYS decrement first!
    
    if (s->count < 0) {
        // Process is BLOCKED
        // Added to waiting queue
    } else {
        // Process proceeds
    }
}
```

### 3. The Signal Operation (V operation)
```c
void sem_signal(struct semaphore *s) {
    s->count++;  // ALWAYS increment first!
    
    if (s->count <= 0) {
        // Wake up one waiting process
    } else {
        // No processes waiting
    }
}
```

## 🎯 Key Points for Your Students

### 1. **Decrement/Increment FIRST**
- `wait()`: Decrement THEN check
- `signal()`: Increment THEN check
- This is crucial! Not the other way around.

### 2. **The Count Variable Meaning**

| count | Meaning |
|-------|---------|
| 1 | 1 resource available, 0 waiting |
| 0 | 0 resources available, 0 waiting |
| -1 | 0 resources available, 1 process waiting |
| -2 | 0 resources available, 2 processes waiting |

**Rule:** Negative count = number of waiting processes!

### 3. **No Busy-Waiting**
- Blocked processes don't spin/loop
- They're removed from CPU
- Only wake up when signaled

## 🚀 Running the Demo

```bash
gcc -o simple_semaphore simple_semaphore.c
./simple_semaphore
```

## 📊 What You'll See

```
Worker 1: count=0, proceeding ✓
Worker 1: IN CRITICAL SECTION

Worker 2: count=-1, BLOCKED ✗
Worker 3: count=-2, BLOCKED ✗

Worker 1: LEAVING, calling signal()
Worker 1: count=-1, waking up one process

Worker 2: Now can enter! IN CRITICAL SECTION
Worker 3: Still waiting...

Worker 2: LEAVING, calling signal()
Worker 2: count=0, waking up one process

Worker 3: Now can enter! IN CRITICAL SECTION
Worker 3: LEAVING, calling signal()
Worker 3: count=1, no process waiting

Back to initial state!
```

## 🎓 Teaching Points

### Show on Board:
```
Initial: count = 1

Process A calls wait():
  count = 1 - 1 = 0
  0 >= 0, so A PROCEEDS ✓

Process B calls wait():
  count = 0 - 1 = -1
  -1 < 0, so B BLOCKS ✗
  Queue: [B]

Process A calls signal():
  count = -1 + 1 = 0
  0 <= 0, so WAKE B ✓
  Queue: []
```

## 🔄 Differences from Real OS Implementation

| This Demo | Real OS |
|-----------|---------|
| Shared memory | Kernel memory |
| `sleep()` to simulate blocking | Real context switch |
| No actual queue | Maintains process queue |
| Single machine | Single machine |
| Educational | Production-grade |

## 💡 Discussion Questions for Class

1. **Why decrement BEFORE checking?**
   - Ensures atomicity
   - Prevents race conditions

2. **What if we didn't block processes?**
   - Busy-waiting (spinning)
   - Wastes CPU cycles
   - Bad performance

3. **What if count started at 3?**
   - Counting semaphore
   - 3 processes can enter simultaneously
   - Useful for resource pools

4. **How is this different from mutex?**
   - Binary semaphore (count=1) IS a mutex
   - Mutex implies ownership (lock/unlock same process)
   - Semaphore can be signaled by different process

## 🔧 Try This Exercise

Modify the program to:
1. Start with `count = 2` instead of 1
2. Run 4 worker processes
3. Observe: 2 can enter simultaneously!

```c
// In main():
sem->count = 2;  // Counting semaphore!
```

Expected behavior:
- Worker 1 and 2: both enter (count = 1, then 0)
- Worker 3: blocks (count = -1)
- Worker 4: blocks (count = -2)
- As workers leave, blocked ones wake up

---

## Summary for Students

**Semaphore = Counter + Waiting Queue**

- **Wait (P)**: Decrement; block if negative
- **Signal (V)**: Increment; wake if processes waiting
- **Count >= 0**: Available resources
- **Count < 0**: Number waiting (absolute value)
- **No busy-waiting**: Blocked processes don't use CPU

This is the foundation of process synchronization in operating systems!

