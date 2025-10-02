#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define TIME_QUANTUM 3

typedef struct {
  int pid;
  int arrival_time;
  int burst_time;
  int remaining_time;
  int completion_time;
  int turnaround_time;
  int waiting_time;
  int response_time;
  int first_run;
} Process;

// Function prototypes
void print_header();
void print_process_details(Process processes[], int n, const char *algorithm);
void calculate_metrics(Process processes[], int n);
void reset_processes(Process original[], Process working[], int n);

// FIFO (First In First Out) Scheduler
void fifo_scheduler(Process processes[], int n) {
  printf("\n=== FIFO Scheduling ===\n");

  // Sort processes by arrival time
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (processes[j].arrival_time > processes[j + 1].arrival_time) {
        Process temp = processes[j];
        processes[j] = processes[j + 1];
        processes[j + 1] = temp;
      }
    }
  }

  int current_time = 0;

  printf("\nExecution Timeline:\n");
  printf("Time | Process | Action\n");
  printf("-----|---------|-------\n");

  for (int i = 0; i < n; i++) {
    // Wait for process to arrive if necessary
    if (current_time < processes[i].arrival_time) {
      printf("%4d | %-7s | CPU Idle\n", current_time, "IDLE");
      current_time = processes[i].arrival_time;
    }

    printf("%4d | P%-6d | Start\n", current_time, processes[i].pid);
    processes[i].response_time = current_time - processes[i].arrival_time;

    // Execute process completely
    current_time += processes[i].burst_time;
    processes[i].completion_time = current_time;

    printf("%4d | P%-6d | Complete\n", current_time, processes[i].pid);
  }

  calculate_metrics(processes, n);
  print_process_details(processes, n, "FIFO");
}

// SJF (Shortest Job First) Scheduler
void sjf_scheduler(Process processes[], int n) {
  printf("\n=== SJF Scheduling ===\n");

  int current_time = 0;
  int completed = 0;
  int is_completed[MAX_PROCESSES] = {0};

  printf("\nExecution Timeline:\n");
  printf("Time | Process | Action\n");
  printf("-----|---------|-------\n");

  while (completed != n) {
    int shortest_job = -1;
    int min_burst = INT_MAX;

    // Find process with shortest burst time among arrived processes
    for (int i = 0; i < n; i++) {
      if (processes[i].arrival_time <= current_time && !is_completed[i] &&
          processes[i].burst_time < min_burst) {
        min_burst = processes[i].burst_time;
        shortest_job = i;
      }
    }

    if (shortest_job == -1) {
      printf("%4d | %-7s | CPU Idle\n", current_time, "IDLE");
      current_time++;
      continue;
    }

    // Execute the shortest job
    printf("%4d | P%-6d | Start\n", current_time, processes[shortest_job].pid);
    processes[shortest_job].response_time =
        current_time - processes[shortest_job].arrival_time;

    current_time += processes[shortest_job].burst_time;
    processes[shortest_job].completion_time = current_time;
    is_completed[shortest_job] = 1;
    completed++;

    printf("%4d | P%-6d | Complete\n", current_time,
           processes[shortest_job].pid);
  }

  calculate_metrics(processes, n);
  print_process_details(processes, n, "SJF");
}

// STCF (Shortest Time to Completion First) - Preemptive SJF
void stcf_scheduler(Process processes[], int n) {
  printf("\n=== STCF (Preemptive SJF) Scheduling ===\n");

  int current_time = 0;
  int completed = 0;
  int is_completed[MAX_PROCESSES] = {0};
  int current_process = -1;

  printf("\nExecution Timeline:\n");
  printf("Time | Process | Remaining | Action\n");
  printf("-----|---------|-----------|--------\n");

  while (completed != n) {
    int shortest_remaining = -1;
    int min_remaining = INT_MAX;

    // Find process with shortest remaining time
    for (int i = 0; i < n; i++) {
      if (processes[i].arrival_time <= current_time && !is_completed[i] &&
          processes[i].remaining_time < min_remaining) {
        min_remaining = processes[i].remaining_time;
        shortest_remaining = i;
      }
    }

    if (shortest_remaining == -1) {
      printf("%4d | %-7s | %-9s | CPU Idle\n", current_time, "IDLE", "-");
      current_time++;
      continue;
    }

    // Check for preemption
    if (current_process != shortest_remaining) {
      if (current_process != -1) {
        printf("%4d | P%-6d | %-9d | Preempted\n", current_time,
               processes[current_process].pid,
               processes[current_process].remaining_time);
      }
      current_process = shortest_remaining;

      // Set response time for first run
      if (processes[current_process].first_run == -1) {
        processes[current_process].first_run = current_time;
        processes[current_process].response_time =
            current_time - processes[current_process].arrival_time;
      }

      printf("%4d | P%-6d | %-9d | Start/Resume\n", current_time,
             processes[current_process].pid,
             processes[current_process].remaining_time);
    }

    // Execute for 1 time unit
    processes[current_process].remaining_time--;
    current_time++;

    // Check if process completed
    if (processes[current_process].remaining_time == 0) {
      processes[current_process].completion_time = current_time;
      is_completed[current_process] = 1;
      completed++;
      printf("%4d | P%-6d | %-9d | Complete\n", current_time,
             processes[current_process].pid, 0);
      current_process = -1;
    }
  }

  calculate_metrics(processes, n);
  print_process_details(processes, n, "STCF");
}

// RR (Round Robin) Scheduler
void rr_scheduler(Process processes[], int n) {
  printf("\n=== Round Robin Scheduling (Quantum = %d) ===\n", TIME_QUANTUM);

  int current_time = 0;
  int completed = 0;
  int queue[MAX_PROCESSES * 10]; // Process queue
  int front = 0, rear = 0;
  int is_in_queue[MAX_PROCESSES] = {0};
  int is_completed[MAX_PROCESSES] = {0};

  printf("\nExecution Timeline:\n");
  printf("Time | Process | Remaining | Action\n");
  printf("-----|---------|-----------|--------\n");

  // Add processes that arrive at time 0 to queue
  for (int i = 0; i < n; i++) {
    if (processes[i].arrival_time <= current_time) {
      queue[rear++] = i;
      is_in_queue[i] = 1;
    }
  }

  while (completed < n) {
    if (front == rear) {
      // Queue is empty, advance time
      printf("%4d | %-7s | %-9s | CPU Idle\n", current_time, "IDLE", "-");
      current_time++;

      // Check for new arrivals
      for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time <= current_time && !is_in_queue[i] &&
            !is_completed[i]) {
          queue[rear++] = i;
          is_in_queue[i] = 1;
        }
      }
      continue;
    }

    // Get next process from queue
    int current_process = queue[front++];
    is_in_queue[current_process] = 0;

    // Set response time for first run
    if (processes[current_process].first_run == -1) {
      processes[current_process].first_run = current_time;
      processes[current_process].response_time =
          current_time - processes[current_process].arrival_time;
    }

    printf("%4d | P%-6d | %-9d | Start\n", current_time,
           processes[current_process].pid,
           processes[current_process].remaining_time);

    // Execute for time quantum or until completion
    int execution_time =
        (processes[current_process].remaining_time < TIME_QUANTUM)
            ? processes[current_process].remaining_time
            : TIME_QUANTUM;

    processes[current_process].remaining_time -= execution_time;
    current_time += execution_time;

    // Check for new arrivals during execution
    for (int i = 0; i < n; i++) {
      if (processes[i].arrival_time <= current_time && !is_in_queue[i] &&
          !is_completed[i] && i != current_process) {
        queue[rear++] = i;
        is_in_queue[i] = 1;
      }
    }

    if (processes[current_process].remaining_time == 0) {
      // Process completed
      processes[current_process].completion_time = current_time;
      is_completed[current_process] = 1;
      completed++;
      printf("%4d | P%-6d | %-9d | Complete\n", current_time,
             processes[current_process].pid, 0);
    } else {
      // Process preempted, add back to queue
      queue[rear++] = current_process;
      is_in_queue[current_process] = 1;
      printf("%4d | P%-6d | %-9d | Preempted\n", current_time,
             processes[current_process].pid,
             processes[current_process].remaining_time);
    }
  }

  calculate_metrics(processes, n);
  print_process_details(processes, n, "Round Robin");
}

void calculate_metrics(Process processes[], int n) {
  for (int i = 0; i < n; i++) {
    processes[i].turnaround_time =
        processes[i].completion_time - processes[i].arrival_time;
    processes[i].waiting_time =
        processes[i].turnaround_time - processes[i].burst_time;
  }
}

void print_process_details(Process processes[], int n, const char *algorithm) {
  printf("\n%s Results:\n", algorithm);
  printf(
      "PID | Arrival | Burst | Completion | Turnaround | Waiting | Response\n");
  printf("----|---------|-------|------------|------------|---------|----------"
         "\n");

  float avg_turnaround = 0, avg_waiting = 0, avg_response = 0;

  for (int i = 0; i < n; i++) {
    printf("P%-2d | %-7d | %-5d | %-10d | %-10d | %-7d | %-8d\n",
           processes[i].pid, processes[i].arrival_time, processes[i].burst_time,
           processes[i].completion_time, processes[i].turnaround_time,
           processes[i].waiting_time, processes[i].response_time);

    avg_turnaround += processes[i].turnaround_time;
    avg_waiting += processes[i].waiting_time;
    avg_response += processes[i].response_time;
  }

  printf("\nAverage Turnaround Time: %.2f\n", avg_turnaround / n);
  printf("Average Waiting Time: %.2f\n", avg_waiting / n);
  printf("Average Response Time: %.2f\n", avg_response / n);
}

void reset_processes(Process original[], Process working[], int n) {
  for (int i = 0; i < n; i++) {
    working[i] = original[i];
    working[i].remaining_time = original[i].burst_time;
    working[i].first_run = -1;
    working[i].completion_time = 0;
    working[i].turnaround_time = 0;
    working[i].waiting_time = 0;
    working[i].response_time = 0;
  }
}

int main() {
  int n;

  printf("Scheduling Algorithms Simulator\n");
  printf("===============================\n");
  printf("Enter number of processes (max %d): ", MAX_PROCESSES);
  scanf("%d", &n);

  if (n > MAX_PROCESSES || n <= 0) {
    printf("Invalid number of processes!\n");
    return 1;
  }

  Process original_processes[MAX_PROCESSES];
  Process working_processes[MAX_PROCESSES];

  // Input process details
  for (int i = 0; i < n; i++) {
    printf("Enter details for Process %d:\n", i + 1);
    original_processes[i].pid = i + 1;
    printf("  Arrival time: ");
    scanf("%d", &original_processes[i].arrival_time);
    printf("  Burst time: ");
    scanf("%d", &original_processes[i].burst_time);
    original_processes[i].remaining_time = original_processes[i].burst_time;
    original_processes[i].first_run = -1;
  }

  int choice;
  do {
    printf("\n\nSelect Scheduling Algorithm:\n");
    printf("1. FIFO (First In First Out)\n");
    printf("2. SJF (Shortest Job First)\n");
    printf("3. STCF (Shortest Time to Completion First)\n");
    printf("4. RR (Round Robin)\n");
    printf("5. Compare All Algorithms\n");
    printf("0. Exit\n");
    printf("Choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      reset_processes(original_processes, working_processes, n);
      fifo_scheduler(working_processes, n);
      break;
    case 2:
      reset_processes(original_processes, working_processes, n);
      sjf_scheduler(working_processes, n);
      break;
    case 3:
      reset_processes(original_processes, working_processes, n);
      stcf_scheduler(working_processes, n);
      break;
    case 4:
      reset_processes(original_processes, working_processes, n);
      rr_scheduler(working_processes, n);
      break;
    case 5:
      printf("\n");
      printf("================================================================="
             "===============\n");
      printf("COMPARING ALL SCHEDULING ALGORITHMS\n");
      printf("================================================================="
             "===============\n");

      // Run all algorithms
      reset_processes(original_processes, working_processes, n);
      fifo_scheduler(working_processes, n);

      reset_processes(original_processes, working_processes, n);
      sjf_scheduler(working_processes, n);

      reset_processes(original_processes, working_processes, n);
      stcf_scheduler(working_processes, n);

      reset_processes(original_processes, working_processes, n);
      rr_scheduler(working_processes, n);
      break;
    case 0:
      printf("Exiting...\n");
      break;
    default:
      printf("Invalid choice!\n");
    }
  } while (choice != 0);

  return 0;
}