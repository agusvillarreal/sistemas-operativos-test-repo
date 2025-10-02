# Module 3: CPU Scheduling
## Lecture Notes

---

## 📚 Table of Contents
1. [Introduction to Scheduling](#introduction)
2. [Scheduling Metrics](#metrics)
3. [FIFO Scheduling](#fifo)
4. [Shortest Job First (SJF)](#sjf)
5. [Shortest Time-to-Completion First (STCF)](#stcf)
6. [Round Robin (RR)](#round-robin)
7. [Multi-Level Feedback Queue (MLFQ)](#mlfq)
8. [Linux Completely Fair Scheduler (CFS)](#cfs)
9. [Real-Time Scheduling](#real-time)

---

## 1. Introduction to Scheduling {#introduction}

### What is CPU Scheduling?

**CPU Scheduling** is the process of determining which process runs on the CPU at any given time.

### Why Do We Need Scheduling?

1. **Maximize CPU utilization**
2. **Fairness** - all processes get CPU time
3. **Responsiveness** - interactive processes respond quickly
4. **Throughput** - complete more jobs per unit time
5. **Minimize wait time** - reduce time in ready queue

### Scheduler Goals

Different workloads have different goals:

| Workload Type | Primary Goal |
|---------------|--------------|
| Batch | Maximize throughput, minimize turnaround time |
| Interactive | Minimize response time, be fair |
| Real-time | Meet deadlines, be predictable |

### Scheduling Assumptions

**Early assumptions** (we'll relax these):
1. Each job runs for the same amount of time
2. All jobs arrive at the same time
3. Once started, each job runs to completion
4. All jobs only use the CPU (no I/O)
5. Run-time of each job is known

---

## 2. Scheduling Metrics {#metrics}

### Key Metrics

#### 1. Turnaround Time
```
T_turnaround = T_completion - T_arrival
```
Time from job arrival to completion.

#### 2. Response Time
```
T_response = T_first_run - T_arrival
```
Time from arrival to first time scheduled.

#### 3. Waiting Time
```
T_waiting = T_turnaround - T_burst
```
Time spent in ready queue.

#### 4. CPU Utilization
```
CPU% = (Busy_time / Total_time) × 100
```
Percentage of time CPU is doing useful work.

#### 5. Throughput
```
Throughput = Jobs_completed / Time
```
Number of jobs completed per unit time.

#### 6. Fairness
```
Jain's Fairness Index = (Σx_i)² / (n × Σx_i²)
```
Where x_i is turnaround time for job i.  
Result: 0 (unfair) to 1 (perfectly fair)

### Example Calculation

Consider 3 jobs:
- A: arrival=0, burst=5
- B: arrival=1, burst=3  
- C: arrival=2, burst=8

If they run in order A→B→C:
- A: T_completion=5, T_turnaround=5-0=5
- B: T_completion=8, T_turnaround=8-1=7
- C: T_completion=16, T_turnaround=16-2=14

Average turnaround time: (5+7+14)/3 = 8.67

---

## 3. FIFO Scheduling {#fifo}

### First In, First Out (FIFO)

**Also known as:** First Come First Served (FCFS)

**Algorithm:** Run jobs in the order they arrive.

### Characteristics

- **Non-preemptive**: Once a job starts, it runs to completion
- **Simple**: Easy to implement (just a queue)
- **Fair**: Every job eventually runs
- **Problem**: Convoy effect

### Convoy Effect

**Problem:** Short jobs wait behind long jobs

```
Time:  0    10   20   30   40   50
       ├────┼────┼────┼────┼────┤
       │    A (40)                │ B(5) │ C(5) │
       └──────────────────────────┴──────┴──────┘

If A arrives first (long job):
- A completes at 40 (turnaround = 40)
- B completes at 45 (turnaround = 44 if arrived at 1)
- C completes at 50 (turnaround = 48 if arrived at 2)

Average turnaround = (40 + 44 + 48) / 3 = 44

But if short jobs first:
- B completes at 5 (turnaround = 4)
- C completes at 10 (turnaround = 8)
- A completes at 50 (turnaround = 50)

Average turnaround = (4 + 8 + 50) / 3 = 20.67
```

### Implementation

```c
void schedule_fifo(process_t *processes, int n) {
    // Sort by arrival time
    qsort(processes, n, sizeof(process_t), compare_arrival);
    
    int current_time = 0;
    
    for (int i = 0; i < n; i++) {
        // Wait for process to arrive
        if (current_time < processes[i].arrival) {
            current_time = processes[i].arrival;
        }
        
        // Record start time (for response time)
        processes[i].start_time = current_time;
        
        // Run to completion
        current_time += processes[i].burst_time;
        
        // Record completion
        processes[i].completion_time = current_time;
    }
}
```

### Pros and Cons

**Pros:**
- Simple to understand and implement
- No starvation (all jobs eventually run)
- Low overhead

**Cons:**
- Poor average turnaround time (convoy effect)
- Not suitable for interactive systems
- No consideration of job length

---

## 4. Shortest Job First (SJF) {#sjf}

### Algorithm

**Run the shortest job first** (of those that have arrived).

### Characteristics

- **Non-preemptive**: Job runs to completion once started
- **Optimal**: Minimizes average turnaround time (for non-preemptive)
- **Problem**: Requires knowing job length
- **Problem**: Starvation possible

### Example

```
Jobs: A(arrival=0, burst=7), B(arrival=2, burst=4), C(arrival=4, burst=1), D(arrival=5, burst=4)

Timeline:
Time:  0    5    10   15   20
       ├────┼────┼────┼────┤
       │  A(7)  │C│  B(4) │  D(4) │
       └────────┴─┴───────┴───────┘

Execution order: A(arrives first) → C(shortest arrived) → B → D

Turnaround times:
- A: 7 - 0 = 7
- B: 15 - 2 = 13
- C: 8 - 4 = 4
- D: 20 - 5 = 15

Average: (7 + 13 + 4 + 15) / 4 = 9.75
```

### Why is SJF Optimal?

**Proof sketch:**
- Consider any schedule
- If a long job runs before a short job, swapping them reduces average turnaround time
- Therefore, shortest jobs should run first

### The Prediction Problem

**Challenge:** How do we know job length in advance?

**Solutions:**
1. **User estimates** - user provides runtime estimate
2. **Past behavior** - use history to predict
3. **Exponential averaging:**
   ```
   τ_n+1 = α × t_n + (1-α) × τ_n
   ```
   where t_n is actual runtime, τ_n is predicted

### Implementation

```c
void schedule_sjf(process_t *processes, int n) {
    int current_time = 0;
    int completed = 0;
    int *finished = calloc(n, sizeof(int));
    
    while (completed < n) {
        // Find shortest job that has arrived
        int shortest = -1;
        int shortest_burst = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (!finished[i] && 
                processes[i].arrival <= current_time &&
                processes[i].burst_time < shortest_burst) {
                shortest = i;
                shortest_burst = processes[i].burst_time;
            }
        }
        
        if (shortest == -1) {
            // No job available, advance time
            current_time++;
            continue;
        }
        
        // Run the shortest job
        processes[shortest].start_time = current_time;
        current_time += processes[shortest].burst_time;
        processes[shortest].completion_time = current_time;
        finished[shortest] = 1;
        completed++;
    }
    
    free(finished);
}
```

### Starvation Problem

Long jobs can starve if short jobs keep arriving:

```
Time:  0────→────→────→────→
       Short jobs keep arriving
       Long job never runs! (STARVATION)
```

---

## 5. Shortest Time-to-Completion First (STCF) {#stcf}

### Also Known As

- Preemptive Shortest Job First (PSJF)
- Shortest Remaining Time First (SRTF)

### Algorithm

**Run the job with the shortest remaining time**

### Characteristics

- **Preemptive**: Can switch jobs mid-execution
- **Optimal**: Minimizes average turnaround time (provably optimal)
- **Dynamic**: Adapts when new jobs arrive

### Example

```
Jobs: A(arrival=0, burst=5), B(arrival=1, burst=2), C(arrival=2, burst=1)

Without preemption (SJF):
Time:  0    5    7    8
       ├────┼────┼────┤
       │  A(5)  │ C│ B │

With preemption (STCF):
Time:  0  1  2  3  5  7
       ├──┼──┼──┼──┼──┤
       │A │C │B │ A  │

When B arrives at time 1:
- A has 4 time units left
- B needs 2 time units
- Switch to B (shorter remaining time)

Average turnaround:
SJF: (5 + 6 + 5)/3 = 5.33
STCF: (6 + 4 + 1)/3 = 3.67 ✓ Better!
```

### Implementation

```c
void schedule_stcf(process_t *processes, int n) {
    int current_time = 0;
    int completed = 0;
    
    while (completed < n) {
        // Find process with shortest remaining time
        int shortest = -1;
        int shortest_remaining = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 &&
                processes[i].arrival <= current_time &&
                processes[i].remaining_time < shortest_remaining) {
                shortest = i;
                shortest_remaining = processes[i].remaining_time;
            }
        }
        
        if (shortest == -1) {
            current_time++;
            continue;
        }
        
        // Mark first run
        if (processes[shortest].start_time == -1) {
            processes[shortest].start_time = current_time;
        }
        
        // Run for 1 time unit
        processes[shortest].remaining_time--;
        current_time++;
        
        if (processes[shortest].remaining_time == 0) {
            processes[shortest].completion_time = current_time;
            completed++;
        }
    }
}
```

### Turnaround vs Response Time

**Problem with STCF:**
- Excellent turnaround time
- Terrible response time (long jobs starve)

Example:
```
Jobs arrive: A(10), B(10), C(10)

All arrive at time 0, run in order:
Response times: A=0, B=10, C=20
Average response time = 10

Not good for interactive systems!
```

---

## 6. Round Robin (RR) {#round-robin}

### Algorithm

**Run each job for a time quantum, then switch to the next job in the queue**

### Characteristics

- **Preemptive**: Switch after time quantum
- **Fair**: Every job gets CPU time
- **Good response time**: All jobs make progress
- **Parameter**: Time quantum (typically 10-100ms)

### How It Works

```
Jobs: A(5), B(3), C(8)  [all arrive at time 0]
Quantum: 2

Time:  0  2  4  6  8  10 12 14 16
       ├──┼──┼──┼──┼──┼──┼──┼──┤
       │A │B │C │A │B │C │A │C │

Queue evolution:
Start: [A, B, C]
After A runs: [B, C, A*]  (* = remaining time: 3)
After B runs: [C, A*, B*] (* = remaining time: 1)
...
```

### Time Quantum Trade-offs

| Quantum Size | Effect |
|--------------|--------|
| Too small | Too many context switches (overhead) |
| Too large | Approaches FIFO (poor response time) |
| Typical | 10-100ms |

**Rule of thumb:** 80% of CPU bursts should be shorter than the quantum

### Implementation

```c
void schedule_rr(process_t *processes, int n, int quantum) {
    int current_time = 0;
    queue_t *ready_queue = queue_create();
    
    // Add initial processes
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival == 0) {
            queue_push(ready_queue, &processes[i]);
        }
    }
    
    while (!queue_empty(ready_queue)) {
        process_t *current = queue_pop(ready_queue);
        
        // Mark first run
        if (current->start_time == -1) {
            current->start_time = current_time;
        }
        
        // Run for quantum or remaining time
        int run_time = (current->remaining_time < quantum) ? 
                        current->remaining_time : quantum;
        
        current->remaining_time -= run_time;
        current_time += run_time;
        
        // Check for newly arrived processes
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current_time &&
                processes[i].arrival > current_time - run_time &&
                processes[i].remaining_time > 0 &&
                &processes[i] != current) {
                queue_push(ready_queue, &processes[i]);
            }
        }
        
        // Re-add if not finished
        if (current->remaining_time > 0) {
            queue_push(ready_queue, current);
        } else {
            current->completion_time = current_time;
        }
    }
}
```

### Performance Analysis

**Turnaround Time:** Typically worse than SJF/STCF
**Response Time:** Much better! All jobs start quickly

Example:
```
Jobs: A(5), B(5), C(5)  Quantum: 1

Response times: A=0, B=1, C=2
Average response = 1 (vs 5 for FIFO)

Turnaround times: A=13, B=14, C=15
Average turnaround = 14 (vs 10 for FIFO)
```

**Trade-off:** Better responsiveness, worse turnaround

---

## 7. Multi-Level Feedback Queue (MLFQ) {#mlfq}

### Goals

1. Minimize turnaround time (like SJF)
2. Minimize response time (like RR)
3. **Learn** job characteristics without prior knowledge

### Key Idea

**Use multiple queues with different priorities**
- Higher priority = run first
- Jobs move between queues based on behavior

### Basic Rules

```
Rule 1: If Priority(A) > Priority(B), A runs
Rule 2: If Priority(A) == Priority(B), run RR with quantum
Rule 3: New job starts at highest priority
Rule 4: If job uses full time slice, move down a queue
Rule 5: If job gives up CPU before slice expires, stay at same priority
```

### Queue Structure

```
┌─────────────────┐  Highest Priority
│   Queue 0 (Q=1) │  Interactive jobs
├─────────────────┤
│   Queue 1 (Q=2) │  
├─────────────────┤
│   Queue 2 (Q=4) │  CPU-bound jobs
├─────────────────┤  Lowest Priority
```

### Example Evolution

```
Job A (interactive): Short CPU bursts with I/O
Job B (CPU-bound): Long CPU bursts

Time 0: Both start at Q0
  A runs briefly, does I/O → stays at Q0
  B uses full quantum → moves to Q1

Time 10: A returns from I/O
  A at Q0 (higher priority) → runs first
  B at Q1 → waits

Result: Interactive jobs stay at high priority
        CPU-bound jobs sink to low priority
```

### The Starvation Problem

**Problem:** CPU-bound jobs can starve at low priority

**Solution:** Priority boost
```
Rule 6: After time S, move all jobs to highest priority
```

This prevents starvation and adapts to changing workload.

### Gaming the Scheduler

**Attack:** Job issues I/O just before quantum expires to stay at high priority

**Defense:** Track total CPU time at each level
```
Rule 4 (revised): If job uses T time units at priority level,
                  regardless of how many slices, move down
```

### Complete MLFQ Rules

```
Rule 1: If Priority(A) > Priority(B), A runs
Rule 2: If Priority(A) == Priority(B), A and B run in RR
Rule 3: Jobs start at highest priority
Rule 4: If job uses up time allotment at level, priority decreases
Rule 5: After period S, move all jobs to topmost queue
```

### Implementation Sketch

```c
typedef struct {
    queue_t *queues[NUM_LEVELS];
    int quantum[NUM_LEVELS];      // Quantum for each level
    int allotment[NUM_LEVELS];    // Time allotment per level
} mlfq_t;

void schedule_mlfq(mlfq_t *mlfq) {
    int current_time = 0;
    int boost_timer = 0;
    
    while (has_jobs(mlfq)) {
        // Priority boost
        if (boost_timer >= BOOST_INTERVAL) {
            boost_all_to_top(mlfq);
            boost_timer = 0;
        }
        
        // Find highest non-empty queue
        int level = find_highest_queue(mlfq);
        if (level == -1) {
            current_time++;
            continue;
        }
        
        // Run job from that queue
        process_t *job = queue_front(mlfq->queues[level]);
        run_for_quantum(job, mlfq->quantum[level]);
        
        // Adjust priority based on behavior
        if (used_full_quantum(job)) {
            move_to_lower_queue(mlfq, job, level);
        }
        
        current_time += mlfq->quantum[level];
        boost_timer += mlfq->quantum[level];
    }
}
```

---

## 8. Linux Completely Fair Scheduler (CFS) {#cfs}

### Philosophy

**Give each process a "fair" share of CPU time**

"Fair" = proportional to weight (nice value)

### Key Concepts

#### 1. Virtual Runtime (vruntime)
```
vruntime = actual_runtime × (NICE_0_WEIGHT / weight)
```

- Track how much CPU time each process has used
- Adjusted by process priority
- **Schedule process with lowest vruntime**

#### 2. Red-Black Tree

CFS uses a red-black tree ordered by vruntime:
```
        [vruntime=50]
       /             \
  [vruntime=20]   [vruntime=80]
     /        \
[vruntime=10]  [vruntime=30]

Leftmost node (vruntime=10) runs next
```

**Benefits:**
- O(log n) insertion/deletion
- O(1) to find minimum vruntime

#### 3. Scheduling Period

```
sched_period = max(MIN_PERIOD, nr_running × TARGET_LATENCY)
```

Time quantum for each process:
```
timeslice = sched_period × (weight_i / Σweight)
```

### Example

```
Processes:
- A: nice=0, weight=1024
- B: nice=5, weight=335
- C: nice=-5, weight=3121

Total weight = 4480

Time slices (for 48ms period):
- A: 48 × (1024/4480) = 11ms
- B: 48 × (335/4480) = 3.6ms
- C: 48 × (3121/4480) = 33.4ms

After one round:
- A vruntime: 11 × (1024/1024) = 11
- B vruntime: 3.6 × (1024/335) ≈ 11
- C vruntime: 33.4 × (1024/3121) ≈ 11

All vruntimes stay roughly equal = fair!
```

### Nice Values

```
Nice value: -20 (highest) to +19 (lowest)
Default: 0

Weight formula:
weight = 1024 / (1.25^nice)

Examples:
nice=-5: weight ≈ 3121 (3× default)
nice=0:  weight = 1024
nice=5:  weight ≈ 335 (1/3 default)
```

---

## 📝 Summary

| Algorithm | Type | Turnaround | Response | Fairness | Notes |
|-----------|------|------------|----------|----------|-------|
| **FIFO** | Non-preemptive | Poor | Poor | Fair | Simple, convoy effect |
| **SJF** | Non-preemptive | Optimal | Poor | Unfair | Needs job length |
| **STCF** | Preemptive | Optimal | Poor | Unfair | Provably optimal |
| **RR** | Preemptive | Fair | Good | Fair | Time quantum key |
| **MLFQ** | Preemptive | Good | Good | Good | Learns job behavior |
| **CFS** | Preemptive | Good | Good | Excellent | Linux default |

### When to Use What?

- **Batch systems**: SJF or STCF
- **Interactive systems**: RR or MLFQ  
- **General purpose**: MLFQ or CFS
- **Real-time**: Priority-based with guarantees

**Next:** Real-time scheduling and implementation details

