/*
 * scheduler_simulator.c
 *
 * Implements multiple CPU scheduling algorithms:
 * - FIFO (First In First Out)
 * - SJF (Shortest Job First)
 * - STCF (Shortest Time to Completion First)
 * - RR (Round Robin)
 *
 * Compile: gcc -o scheduler scheduler_simulator.c -lm
 * Run: ./scheduler <algorithm> <workload_file>
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
  int pid;
  int arrival_time;
  int burst_time;
  int priority;
  int remaining_time;
  int start_time;
  int completion_time;
  int turnaround_time;
  int waiting_time;
  int response_time;
} process_t;

typedef struct {
  process_t *proc;
  int start;
  int end;
} timeline_t;

timeline_t timeline[1000];
int timeline_count = 0;

/* Add to timeline */
void add_timeline(process_t *proc, int start, int end) {
  if (timeline_count > 0 && timeline[timeline_count - 1].proc == proc &&
      timeline[timeline_count - 1].end == start) {
    // Extend previous entry
    timeline[timeline_count - 1].end = end;
  } else {
    timeline[timeline_count].proc = proc;
    timeline[timeline_count].start = start;
    timeline[timeline_count].end = end;
    timeline_count++;
  }
}

/* Calculate metrics */
void calculate_metrics(process_t *processes, int n) {
  for (int i = 0; i < n; i++) {
    processes[i].turnaround_time =
        processes[i].completion_time - processes[i].arrival_time;
    processes[i].waiting_time =
        processes[i].turnaround_time - processes[i].burst_time;
    processes[i].response_time =
        processes[i].start_time - processes[i].arrival_time;
  }
}

/* Print results */
void print_results(const char *algorithm, process_t *processes, int n) {
  printf("\n=== %s Scheduling Results ===\n", algorithm);
  printf("%-4s %-8s %-6s %-9s %-6s %-6s %-6s\n", "PID", "Arrival", "Burst",
         "Complete", "TAT", "WT", "RT");
  printf("-----------------------------------------------------\n");

  double total_tat = 0, total_wt = 0, total_rt = 0;

  for (int i = 0; i < n; i++) {
    printf("%-4d %-8d %-6d %-9d %-6d %-6d %-6d\n", processes[i].pid,
           processes[i].arrival_time, processes[i].burst_time,
           processes[i].completion_time, processes[i].turnaround_time,
           processes[i].waiting_time, processes[i].response_time);

    total_tat += processes[i].turnaround_time;
    total_wt += processes[i].waiting_time;
    total_rt += processes[i].response_time;
  }

  printf("\nAverage Turnaround Time: %.2f\n", total_tat / n);
  printf("Average Waiting Time:    %.2f\n", total_wt / n);
  printf("Average Response Time:   %.2f\n", total_rt / n);
}

/* Print Gantt chart */
void print_gantt_chart() {
  if (timeline_count == 0)
    return;

  printf("\n=== Gantt Chart ===\n");

  // Print process bars
  for (int i = 0; i < timeline_count; i++) {
    int width = timeline[i].end - timeline[i].start;
    printf("|");
    for (int j = 0; j < width; j++)
      printf("-");
    printf("P%d", timeline[i].proc->pid);
    for (int j = 0; j < width; j++)
      printf("-");
  }
  printf("|\n");

  // Print time markers
  printf("%d", timeline[0].start);
  for (int i = 0; i < timeline_count; i++) {
    int width = (timeline[i].end - timeline[i].start) * 2 + 3;
    for (int j = 0; j < width - 2; j++)
      printf(" ");
    printf("%d", timeline[i].end);
  }
  printf("\n");
}

/* FIFO Scheduling */
void schedule_fifo(process_t *processes, int n) {
  int current_time = 0;

  // Sort by arrival time
  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      if (processes[j].arrival_time < processes[i].arrival_time) {
        process_t temp = processes[i];
        processes[i] = processes[j];
        processes[j] = temp;
      }
    }
  }

  for (int i = 0; i < n; i++) {
    if (current_time < processes[i].arrival_time) {
      current_time = processes[i].arrival_time;
    }

    processes[i].start_time = current_time;
    int end_time = current_time + processes[i].burst_time;

    add_timeline(&processes[i], current_time, end_time);

    processes[i].completion_time = end_time;
    current_time = end_time;
  }

  calculate_metrics(processes, n);
  print_results("FIFO", processes, n);
  print_gantt_chart();
}

/* SJF Scheduling */
void schedule_sjf(process_t *processes, int n) {
  int current_time = 0;
  int completed = 0;
  int *done = calloc(n, sizeof(int));

  while (completed < n) {
    int shortest = -1;
    int shortest_burst = 9999;

    // Find shortest job that has arrived
    for (int i = 0; i < n; i++) {
      if (!done[i] && processes[i].arrival_time <= current_time &&
          processes[i].burst_time < shortest_burst) {
        shortest = i;
        shortest_burst = processes[i].burst_time;
      }
    }

    if (shortest == -1) {
      current_time++;
      continue;
    }

    processes[shortest].start_time = current_time;
    int end_time = current_time + processes[shortest].burst_time;

    add_timeline(&processes[shortest], current_time, end_time);

    processes[shortest].completion_time = end_time;
    current_time = end_time;
    done[shortest] = 1;
    completed++;
  }

  free(done);
  calculate_metrics(processes, n);
  print_results("SJF", processes, n);
  print_gantt_chart();
}

/* STCF Scheduling */
void schedule_stcf(process_t *processes, int n) {
  int current_time = 0;
  int completed = 0;

  // Initialize remaining times
  for (int i = 0; i < n; i++) {
    processes[i].remaining_time = processes[i].burst_time;
    processes[i].start_time = -1;
  }

  while (completed < n) {
    int shortest = -1;
    int shortest_remaining = 9999;

    // Find process with shortest remaining time
    for (int i = 0; i < n; i++) {
      if (processes[i].remaining_time > 0 &&
          processes[i].arrival_time <= current_time &&
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
    int start = current_time;
    processes[shortest].remaining_time--;
    current_time++;

    add_timeline(&processes[shortest], start, current_time);

    if (processes[shortest].remaining_time == 0) {
      processes[shortest].completion_time = current_time;
      completed++;
    }
  }

  calculate_metrics(processes, n);
  print_results("STCF", processes, n);
  print_gantt_chart();
}

/* Round Robin Scheduling */
void schedule_rr(process_t *processes, int n, int quantum) {
  int current_time = 0;
  int completed = 0;

  // Initialize remaining times
  for (int i = 0; i < n; i++) {
    processes[i].remaining_time = processes[i].burst_time;
    processes[i].start_time = -1;
  }

  // Simple queue (circular)
  int queue[MAX_PROCESSES];
  int front = 0, rear = 0, size = 0;

  // Add processes that arrive at time 0
  for (int i = 0; i < n; i++) {
    if (processes[i].arrival_time == 0) {
      queue[rear++] = i;
      size++;
    }
  }

  while (completed < n) {
    if (size == 0) {
      // Find next arrival
      int next_arrival = 9999;
      for (int i = 0; i < n; i++) {
        if (processes[i].remaining_time > 0 &&
            processes[i].arrival_time > current_time) {
          if (processes[i].arrival_time < next_arrival) {
            next_arrival = processes[i].arrival_time;
          }
        }
      }

      if (next_arrival != 9999) {
        current_time = next_arrival;
        for (int i = 0; i < n; i++) {
          if (processes[i].arrival_time == current_time &&
              processes[i].remaining_time > 0) {
            queue[rear++ % MAX_PROCESSES] = i;
            size++;
          }
        }
      }
      continue;
    }

    int idx = queue[front++ % MAX_PROCESSES];
    size--;

    // Mark first run
    if (processes[idx].start_time == -1) {
      processes[idx].start_time = current_time;
    }

    // Run for quantum or remaining time
    int run_time = (processes[idx].remaining_time < quantum)
                       ? processes[idx].remaining_time
                       : quantum;

    int start = current_time;
    processes[idx].remaining_time -= run_time;
    current_time += run_time;

    add_timeline(&processes[idx], start, current_time);

    // Add newly arrived processes
    for (int i = 0; i < n; i++) {
      if (processes[i].arrival_time <= current_time &&
          processes[i].arrival_time > start &&
          processes[i].remaining_time > 0 && i != idx) {
        int already_in_queue = 0;
        for (int j = 0; j < size; j++) {
          if (queue[(front + j) % MAX_PROCESSES] == i) {
            already_in_queue = 1;
            break;
          }
        }
        if (!already_in_queue) {
          queue[rear++ % MAX_PROCESSES] = i;
          size++;
        }
      }
    }

    // Re-add current process if not finished
    if (processes[idx].remaining_time > 0) {
      queue[rear++ % MAX_PROCESSES] = idx;
      size++;
    } else {
      processes[idx].completion_time = current_time;
      completed++;
    }
  }

  calculate_metrics(processes, n);
  print_results("Round Robin", processes, n);
  print_gantt_chart();
}

/* Load processes from file */
int load_workload(const char *filename, process_t *processes) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    perror("fopen");
    return -1;
  }

  int count = 0;
  while (fscanf(fp, "%d %d %d", &processes[count].arrival_time,
                &processes[count].burst_time,
                &processes[count].priority) == 3) {
    processes[count].pid = count + 1;
    count++;
    if (count >= MAX_PROCESSES)
      break;
  }

  fclose(fp);
  return count;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <algorithm> <workload_file> [quantum]\n", argv[0]);
    printf("Algorithms: fifo, sjf, stcf, rr\n");
    printf("Workload format: arrival_time burst_time priority\n");
    return 1;
  }

  process_t processes[MAX_PROCESSES];
  int n = load_workload(argv[2], processes);

  if (n <= 0) {
    printf("Error loading workload\n");
    return 1;
  }

  printf("Loaded %d processes\n", n);

  timeline_count = 0;

  if (strcmp(argv[1], "fifo") == 0) {
    schedule_fifo(processes, n);
  } else if (strcmp(argv[1], "sjf") == 0) {
    schedule_sjf(processes, n);
  } else if (strcmp(argv[1], "stcf") == 0) {
    schedule_stcf(processes, n);
  } else if (strcmp(argv[1], "rr") == 0) {
    int quantum = (argc > 3) ? atoi(argv[3]) : 3;
    printf("Using time quantum: %d\n", quantum);
    schedule_rr(processes, n, quantum);
  } else {
    printf("Unknown algorithm: %s\n", argv[1]);
    return 1;
  }

  return 0;
}

/*
 * EXAMPLE WORKLOAD FILE (workload.txt):
 * 0 5 1
 * 1 3 2
 * 2 8 1
 * 3 6 3
 *
 * USAGE:
 * ./scheduler fifo workload.txt
 * ./scheduler sjf workload.txt
 * ./scheduler stcf workload.txt
 * ./scheduler rr workload.txt 3
 */
