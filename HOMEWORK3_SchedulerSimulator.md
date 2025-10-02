# Homework 3: Scheduler Simulator with GUI
## Standalone Assignment - CPU Scheduling Algorithms

**Due Date:** 3 weeks from assignment date  
**Weight:** 25% of final grade  
**Points:** 100 points  
**Submission:** Source code + README + Makefile + demo video + performance report

---

## 📌 Assignment Overview

This is **Homework 3 of 5** in the Linux Systems Programming course. Each homework is an **independent project**:

- Homework 1: Basic Process Management
- Homework 2: Zombie Process Handler
- **Homework 3** (This): Scheduler Simulator with GUI ← You are here
- Homework 4: Chat System using IPC
- Homework 5: Custom Kernel Module

---

## 🎯 Learning Objectives

By completing this homework, you will:
1. Implement major CPU scheduling algorithms
2. Understand scheduling metrics (turnaround time, response time, fairness)
3. Create visual timeline (Gantt charts)
4. Build interactive GUI application
5. Compare algorithm performance with real data

---

## 📋 Assignment Description

You will implement a **CPU Scheduler Simulator** with graphical interface that:
- Simulates 5+ scheduling algorithms
- Visualizes execution with Gantt charts
- Calculates performance metrics
- Allows interactive parameter tuning
- Compares algorithms side-by-side

---

## 🔧 Requirements

### Part 1: Core Data Structures (10 points)

```c
typedef struct {
    int pid;                    // Process ID
    int arrival_time;           // When process arrives
    int burst_time;             // Total CPU time needed
    int priority;               // Priority (lower = higher priority)
    int remaining_time;         // Time left to execute
    int start_time;             // First time scheduled
    int completion_time;        // When finished
    int turnaround_time;        // completion - arrival
    int waiting_time;           // turnaround - burst
    int response_time;          // start - arrival
} process_t;

typedef struct {
    int time;                   // Time slice start
    int pid;                    // Process running
    int duration;               // How long it ran
} timeline_event_t;
```

---

### Part 2: Scheduling Algorithms (50 points)

Implement the following algorithms:

#### Algorithm 1: FIFO (First In First Out) - 8 points
```c
void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline);
```

**Characteristics:**
- Non-preemptive
- Run processes in arrival order
- Simple but can have long wait times

#### Algorithm 2: SJF (Shortest Job First) - 10 points
```c
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline);
```

**Characteristics:**
- Non-preemptive
- Select shortest burst time among arrived processes
- Minimizes average waiting time
- Can cause starvation

#### Algorithm 3: STCF (Shortest Time to Completion First) - 10 points
```c
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline);
```

**Characteristics:**
- Preemptive version of SJF
- Switch if new process has shorter remaining time
- Optimal for turnaround time

#### Algorithm 4: Round Robin - 10 points
```c
void schedule_rr(process_t *processes, int n, int quantum, 
                 timeline_event_t *timeline);
```

**Characteristics:**
- Preemptive
- Each process gets time quantum
- Fair distribution of CPU
- Good response time
- Quantum size affects performance

#### Algorithm 5: MLFQ (Multi-Level Feedback Queue) - 12 points
```c
typedef struct {
    int num_queues;
    int *quantums;              // Quantum for each queue
    int boost_interval;         // Priority boost every N time units
} mlfq_config_t;

void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config,
                   timeline_event_t *timeline);
```

**Characteristics:**
- Multiple priority queues
- Processes start at highest priority
- Drop priority if use full quantum
- Boost all to top periodically
- Balances interactive and CPU-bound

---

### Part 3: Metrics Calculation (15 points)

Implement metrics calculation:

```c
typedef struct {
    double avg_turnaround_time;
    double avg_waiting_time;
    double avg_response_time;
    double cpu_utilization;
    double throughput;
    double fairness_index;      // Jain's fairness index
} metrics_t;

void calculate_metrics(process_t *processes, int n, int total_time,
                       metrics_t *metrics);
```

**Formulas:**
- Turnaround Time = Completion Time - Arrival Time
- Waiting Time = Turnaround Time - Burst Time
- Response Time = Start Time - Arrival Time
- CPU Utilization = (Busy Time / Total Time) × 100
- Throughput = Processes Completed / Total Time
- Fairness = (Σxi)² / (n × Σxi²)  where xi = turnaround time

---

### Part 4: GUI Implementation (20 points)

Create interactive GUI using **GTK** or **ncurses**:

#### Option A: GTK (Recommended)

```c
// Main window with:
// - Process input table
// - Algorithm selection dropdown
// - Parameter inputs (quantum, priorities)
// - Run button
// - Gantt chart display
// - Metrics display
```

**Required Features:**
1. **Process Editor**
   - Add/remove processes
   - Set arrival time, burst time, priority
   - Load from file / Save to file

2. **Gantt Chart Visualization**
   - Timeline with process execution
   - Different colors per process
   - Hover to show details
   - Time scale (0, 5, 10, 15...)

3. **Metrics Dashboard**
   - Table showing all metrics
   - Comparison between algorithms
   - Highlight best/worst

4. **Interactive Controls**
   - Adjust quantum for RR
   - Adjust MLFQ queues and quantums
   - Real-time re-simulation

#### Option B: ncurses (Terminal UI)

```
┌─────────────────────────────────────────────────────────────┐
│           CPU Scheduler Simulator v1.0                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Processes:                                                 │
│  ┌─────┬─────────┬──────────┬──────────┐                   │
│  │ PID │ Arrival │ Burst    │ Priority │                   │
│  ├─────┼─────────┼──────────┼──────────┤                   │
│  │  1  │    0    │    5     │    1     │                   │
│  │  2  │    1    │    3     │    2     │                   │
│  │  3  │    2    │    8     │    1     │                   │
│  └─────┴─────────┴──────────┴──────────┘                   │
│                                                             │
│  Algorithm: [Round Robin]  Quantum: [3]                    │
│                                                             │
│  Gantt Chart:                                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ P1 │ P2│ P3   │ P1│ P2│ P3   │ P1│ P3   │          │   │
│  ├────┴───┴──────┴───┴───┴──────┴───┴──────┘          │   │
│  0    5   8     11  14  17     20  22     30          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  Metrics:                                                   │
│  Avg Turnaround Time: 13.67                                │
│  Avg Waiting Time:     8.33                                │
│  Avg Response Time:    2.33                                │
│  CPU Utilization:     97.3%                                │
│                                                             │
│  [R]un  [A]dd Process  [D]elete  [S]ave  [L]oad  [Q]uit  │
└─────────────────────────────────────────────────────────────┘
```

---

### Part 5: Comparison & Analysis (5 points)

Generate comparison report:

```c
void generate_report(const char *filename, process_t *processes, int n);
```

**Report Format (Markdown or HTML):**
```markdown
# Scheduler Performance Report

## Process Set
| PID | Arrival | Burst | Priority |
|-----|---------|-------|----------|
| 1   | 0       | 5     | 1        |
| 2   | 1       | 3     | 2        |
| 3   | 2       | 8     | 1        |

## Algorithm Comparison

| Algorithm | Avg TAT | Avg WT | Avg RT | Throughput |
|-----------|---------|--------|--------|------------|
| FIFO      | 12.33   | 7.00   | 7.00   | 0.19       |
| SJF       | 10.67   | 5.33   | 4.00   | 0.19       |
| STCF      | 8.33    | 3.00   | 2.67   | 0.19       |
| RR (q=3)  | 13.67   | 8.33   | 2.33   | 0.19       |
| MLFQ      | 11.00   | 5.67   | 3.00   | 0.19       |

## Best Algorithm for This Workload
**STCF** - Lowest average turnaround time and waiting time

## Recommendations
- Interactive processes: Use MLFQ or RR
- Batch jobs: Use SJF or STCF
- Mixed workload: Use MLFQ with appropriate tuning
```

---

## 📦 Deliverables

```
homework3/
├── src/
│   ├── scheduler.c             # Main simulator
│   ├── algorithms.c            # All 5 algorithms
│   ├── metrics.c               # Metrics calculation
│   ├── gui_gtk.c               # GTK GUI (or)
│   ├── gui_ncurses.c           # ncurses UI
│   └── report.c                # Report generator
├── include/
│   ├── scheduler.h
│   ├── algorithms.h
│   └── metrics.h
├── tests/
│   ├── test_fifo.c
│   ├── test_sjf.c
│   ├── test_stcf.c
│   ├── test_rr.c
│   └── test_mlfq.c
├── workloads/
│   ├── workload1.txt           # Test process sets
│   ├── workload2.txt
│   └── workload3.txt
├── docs/
│   ├── DESIGN.md               # Design decisions
│   ├── ALGORITHMS.md           # Algorithm explanations
│   └── ANALYSIS.pdf            # Performance analysis
├── Makefile
├── README.md
└── demo.mp4                    # Video demonstration
```

---

## 📊 Grading Rubric

| Component | Points | Criteria |
|-----------|--------|----------|
| **Part 1: Data Structures** | 10 | |
| - Proper structs | 5 | Complete fields |
| - Timeline events | 5 | Tracks execution |
| **Part 2: Algorithms** | 50 | |
| - FIFO | 8 | Correct implementation |
| - SJF | 10 | Handles arrivals |
| - STCF | 10 | Preemption works |
| - Round Robin | 10 | Quantum respected |
| - MLFQ | 12 | Multiple queues, boosting |
| **Part 3: Metrics** | 15 | |
| - Calculations | 10 | All metrics correct |
| - Fairness index | 5 | Proper formula |
| **Part 4: GUI** | 20 | |
| - Interface design | 5 | Clean, usable |
| - Gantt chart | 8 | Visual, clear |
| - Interactive | 5 | Parameters adjustable |
| - Responsiveness | 2 | No lag |
| **Part 5: Report** | 5 | |
| - Comparison table | 3 | All algorithms |
| - Analysis | 2 | Insightful |
| **Code Quality** | 10 | |
| - Modular design | 3 | Separated concerns |
| - Error handling | 2 | Checks inputs |
| - Comments | 3 | Clear docs |
| - Memory management | 2 | No leaks |
| **Documentation** | 10 | |
| - README | 4 | Complete guide |
| - Design doc | 3 | Explains choices |
| - Demo video | 3 | Shows all features |

**Total: 100 points**

**Bonus:** 
- Real-time animation (+5 points)
- CFS implementation (+10 points)
- Save/load workloads (+3 points)

---

## 🧪 Test Workloads

### Workload 1: Simple (3 processes)
```
# arrival burst priority
0 5 1
1 3 2
2 8 1
```

### Workload 2: Convoy Effect
```
0 100 1
1 1 2
2 1 3
```

### Workload 3: Mixed (10 processes)
```
0 8 1
1 4 2
2 9 1
3 5 3
4 2 1
5 6 2
6 3 1
7 7 3
8 4 2
9 1 1
```

---

## 💡 Implementation Tips

### Gantt Chart with GTK

```c
cairo_t *cr = // from drawing area
cairo_set_source_rgb(cr, 0.2, 0.4, 0.8);  // Blue for P1
cairo_rectangle(cr, 0, 0, 50, 30);         // x, y, width, height
cairo_fill(cr);
```

### Priority Queue for SJF

```c
typedef struct {
    process_t *processes[MAX_PROCESSES];
    int size;
} pqueue_t;

void pqueue_insert(pqueue_t *pq, process_t *p) {
    // Insert in sorted order by burst_time
}

process_t *pqueue_pop(pqueue_t *pq) {
    // Remove and return shortest job
}
```

---

## 📚 Resources

- **OSTEP Book**: Chapters 7-9 (Scheduling)
- **GTK Tutorial**: https://www.gtk.org/docs/getting-started/
- **ncurses Guide**: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
- **Scheduling Papers**: Look up "Completely Fair Scheduler" (CFS)

---

## ❓ FAQ

**Q: Can I use Python instead of C?**  
A: No, this course requires C for system-level programming.

**Q: Do I need to implement all 5 algorithms?**  
A: Yes, all 5 are required for full credit.

**Q: Can I use a different GUI library?**  
A: GTK or ncurses only. No Qt, no SDL.

**Q: How do I handle process arrival times?**  
A: Sort by arrival time initially, then check at each time unit which processes have arrived.

---

**Good luck! This is a challenging but rewarding project!** 📊🎨

