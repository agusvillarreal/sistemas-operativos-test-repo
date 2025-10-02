# Module 3: CPU Scheduling

Complete teaching materials for CPU scheduling algorithms.

## 📁 Contents

- `LECTURE_NOTES.md` - Complete theory for all scheduling algorithms
- `examples/scheduler_simulator.c` - Working scheduler implementations
- `examples/Makefile` - Build system

## 🎯 Learning Objectives

Students will:
1. Understand scheduling metrics (turnaround, response, waiting time)
2. Implement FIFO, SJF, STCF, and Round Robin
3. Analyze algorithm trade-offs
4. Compare performance on different workloads
5. Build GUI scheduler simulator (Homework 3)

## 🚀 Quick Start

```bash
cd examples/
make
make test  # Run all algorithms on test workload
```

## 📚 Teaching Guide

### Week 5: Basic Scheduling

**Day 1-2:** Metrics and FIFO/SJF
- Lecture: LECTURE_NOTES.md sections 1-4
- Demo: scheduler_simulator with FIFO and SJF
- Lab: Students implement FIFO

**Day 3-4:** Preemptive Scheduling
- Lecture: LECTURE_NOTES.md sections 5-6
- Demo: STCF and Round Robin
- Lab: Students implement Round Robin

**Day 5:** Assign Homework 3
- Release: Scheduler Simulator with GUI
- Due in 3 weeks

### Week 6: Advanced Scheduling

**Day 1-2:** MLFQ
- Lecture: LECTURE_NOTES.md section 7
- Discussion: Gaming the scheduler
- Lab: MLFQ simulation

**Day 3-4:** Linux CFS
- Lecture: LECTURE_NOTES.md section 8
- Demo: nice values and priorities
- Lab: Measure CFS behavior

**Day 5:** Algorithm Comparison
- Compare all algorithms
- Performance analysis
- Real-world considerations

## ✅ Homework 3

See `HOMEWORK3_SchedulerSimulator.md` for complete assignment:
- Implement 5+ scheduling algorithms
- Create GUI with Gantt charts
- Performance metrics and analysis
- Worth 100 points (25% of grade)

## 🎓 Ready to Use!

All materials complete and tested.

