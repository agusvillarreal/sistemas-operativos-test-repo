# Simple Semaphore Implementation Explained

## The Structure

```c
struct semaphore {
    int count;  // number of available resources
    queue q;    // waiting processes queue
};
```

**Key Points:**
- `count` tracks available resources (1 for mutex, N for counting semaphore)
- `queue q` holds processes waiting for the resource
- This structure is shared between all processes

---

## Wait Operation (P, down, acquire)

```c
void wait(semaphore *s) {
    s->count--;               // decrement count
    
    if (s->count < 0) {
        // Remove one process from the waiting queue
        // and wake it up
    }
}
```

**What happens:**
1. **Decrement first**: `count--` (always happens!)
2. **Check**: If `count < 0` after decrement:
   - Process is BLOCKED
   - Added to waiting queue
   - Removed from CPU (context switch)
3. **If `count >= 0`**: Process continues immediately

**Example with count=1 (mutex):**
- Process A calls `wait()`: count becomes 0 → proceeds ✓
- Process B calls `wait()`: count becomes -1 → BLOCKS ✗
- Process C calls `wait()`: count becomes -2 → BLOCKS ✗

---

## Signal Operation (V, up, release)

```c
void signal(semaphore *s) {
    s->count++;               // increment count
    
    if (s->count <= 0) {
        // Remove one process from waiting queue
        // and wake it up
    }
}
```

**What happens:**
1. **Increment first**: `count++` (always happens!)
2. **Check**: If `count <= 0` after increment:
   - There are still waiting processes
   - Wake up ONE process from queue
   - Move it to ready queue
3. **If `count > 0`**: No processes waiting

**Continuing example:**
- Process A calls `signal()`: count becomes -1 → wakes Process B
- Process B runs, calls `signal()`: count becomes 0 → wakes Process C

---

## Why This Design?

### The count variable tells us TWO things:

**When count >= 0:**
- Value = number of available resources
- No processes are waiting

**When count < 0:**
- Absolute value = number of waiting processes
- No resources available

### Examples:

| count | Meaning |
|-------|---------|
| 3 | 3 resources available, 0 waiting |
| 1 | 1 resource available, 0 waiting |
| 0 | 0 resources available, 0 waiting |
| -1 | 0 resources available, 1 waiting |
| -3 | 0 resources available, 3 waiting |

---

## Visual Example: 3 Processes with Binary Semaphore

```
Initial state: count = 1

Time 1: Process A calls wait()
        count = 1 - 1 = 0
        0 >= 0, so A proceeds ✓
        A enters critical section

Time 2: Process B calls wait()
        count = 0 - 1 = -1
        -1 < 0, so B BLOCKS ✗
        B added to queue: [B]

Time 3: Process C calls wait()
        count = -1 - 1 = -2
        -2 < 0, so C BLOCKS ✗
        C added to queue: [B, C]

Time 4: Process A calls signal()
        count = -2 + 1 = -1
        -1 <= 0, so wake one process
        Remove B from queue, B can run now
        Queue: [C]

Time 5: Process B calls signal()
        count = -1 + 1 = 0
        0 <= 0, so wake one process
        Remove C from queue, C can run now
        Queue: []

Time 6: Process C calls signal()
        count = 0 + 1 = 1
        1 > 0, no processes waiting
        Back to initial state!
```

---

## Key Properties

### 1. Atomicity
- `count--` and `count++` must be ATOMIC operations
- No two processes can modify count simultaneously
- OS uses special instructions or disables interrupts

### 2. No Busy-Waiting
- Blocked processes don't consume CPU
- They're removed from ready queue
- Only woken up when resource is available

### 3. FIFO Fairness (usually)
- Processes typically served in order they arrive
- Prevents starvation
- Implementation-dependent

---

## Comparison: Binary vs Counting Semaphore

### Binary Semaphore (Mutex)
```c
semaphore mutex;
mutex.count = 1;  // Only 1 resource
```
- Acts as a lock
- Only 1 process in critical section
- Common use: protect shared data

### Counting Semaphore
```c
semaphore resources;
resources.count = 5;  // 5 resources available
```
- Allows N processes simultaneously
- Tracks resource availability
- Common use: thread pools, connection pools

---

## Common Mistakes

### ❌ Wrong: Check then decrement
```c
if (s->count > 0) {  // Race condition here!
    s->count--;
    proceed();
}
```

### ✓ Correct: Decrement then check
```c
s->count--;           // Atomic operation
if (s->count < 0) {
    block();
}
```

The key is that decrement happens FIRST and ATOMICALLY!

---

## Demo Notes

In `simple_semaphore.c`:
- We use shared memory so all processes see same semaphore
- We simulate blocking with `sleep()` (real OS would context switch)
- We don't actually implement the queue (real OS would)
- This shows the CONCEPT, not production code!

**To run:**
```bash
gcc -o simple_semaphore simple_semaphore.c
./simple_semaphore
```

Watch how only one process enters the critical section at a time!

