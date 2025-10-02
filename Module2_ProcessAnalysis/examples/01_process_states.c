/*
 * 01_process_states.c
 *
 * Demonstrates different process states in Linux
 * Shows how to create processes in various states
 *
 * Compile: gcc -o process_states 01_process_states.c
 * Run: ./process_states
 * Monitor: ps aux | grep process_states (in another terminal)
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Helper: Print process state */
void print_state(pid_t pid, const char *label) {
  char path[256], state;
  FILE *fp;

  snprintf(path, sizeof(path), "/proc/%d/stat", pid);
  fp = fopen(path, "r");

  if (fp) {
    fscanf(fp, "%*d %*s %c", &state);
    fclose(fp);

    const char *state_desc;
    switch (state) {
    case 'R':
      state_desc = "Running/Runnable";
      break;
    case 'S':
      state_desc = "Sleeping (interruptible)";
      break;
    case 'D':
      state_desc = "Disk sleep (uninterruptible)";
      break;
    case 'Z':
      state_desc = "Zombie";
      break;
    case 'T':
      state_desc = "Stopped";
      break;
    case 't':
      state_desc = "Tracing stop";
      break;
    default:
      state_desc = "Unknown";
    }

    printf("[%s] PID %d: State '%c' - %s\n", label, pid, state, state_desc);
  } else {
    printf("[%s] PID %d: Could not read state\n", label, pid);
  }
}

/* Example 1: Running/Runnable State */
void example_running() {
  printf("\n=== Example 1: Running/Runnable State ===\n");
  printf("This process is in R state (running or ready to run)\n");

  pid_t pid = getpid();
  print_state(pid, "Current");

  printf("Doing CPU-intensive work...\n");
  volatile long sum = 0;
  for (long i = 0; i < 100000000; i++) {
    sum += i;
  }

  print_state(pid, "After CPU work");
  printf("(Check with: ps aux | grep %d)\n", pid);
}

/* Example 2: Sleeping State (Interruptible) */
void example_sleeping() {
  printf("\n=== Example 2: Sleeping State (S) ===\n");

  pid_t pid = fork();

  if (pid == 0) {
    // Child will sleep
    printf("[Child %d] Going to sleep for 5 seconds...\n", getpid());
    print_state(getpid(), "Before sleep");

    sleep(5); // Interruptible sleep - state S

    print_state(getpid(), "After sleep");
    printf("[Child %d] Woke up!\n", getpid());
    exit(0);
  } else {
    // Parent monitors child
    sleep(1); // Give child time to sleep
    printf("[Parent] Monitoring child %d:\n", pid);
    print_state(pid, "Child");

    printf("(Check with: ps aux | grep %d)\n", pid);
    printf("Child should show 'S' state\n");

    wait(NULL);
  }
}

/* Example 3: Zombie State */
void example_zombie() {
  printf("\n=== Example 3: Zombie State (Z) ===\n");
  printf("Creating a zombie process (will last 5 seconds)\n");

  pid_t pid = fork();

  if (pid == 0) {
    // Child exits immediately
    printf("[Child %d] Exiting to become zombie...\n", getpid());
    exit(42);
  } else {
    // Parent doesn't call wait() yet
    sleep(2); // Child is now zombie

    printf("[Parent] Child %d should be zombie now\n", pid);
    print_state(pid, "Child");

    printf("(Check with: ps aux | grep %d)\n", pid);
    printf("You should see '<defunct>' or 'Z' state\n");

    sleep(3);

    // Now reap the zombie
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      printf("[Parent] Reaped zombie, exit status: %d\n",
             WEXITSTATUS(status));
    }

    // Verify zombie is gone
    if (kill(pid, 0) == -1 && errno == ESRCH) {
      printf("[Parent] Zombie successfully reaped (process gone)\n");
    }
  }
}

/* Example 4: Stopped State */
void example_stopped() {
  printf("\n=== Example 4: Stopped State (T) ===\n");

  pid_t pid = fork();

  if (pid == 0) {
    // Child will be stopped
    printf("[Child %d] Waiting to be stopped...\n", getpid());

    // Busy loop so we can stop it
    while (1) {
      sleep(1);
    }
  } else {
    sleep(1); // Let child start

    // Send SIGSTOP to child
    printf("[Parent] Stopping child %d with SIGSTOP\n", pid);
    kill(pid, SIGSTOP);

    sleep(1);
    print_state(pid, "Stopped child");
    printf("(Check with: ps aux | grep %d)\n", pid);
    printf("Child should show 'T' state\n");

    // Continue the child
    printf("[Parent] Resuming child with SIGCONT\n");
    kill(pid, SIGCONT);

    sleep(1);
    print_state(pid, "Resumed child");

    // Kill the child
    kill(pid, SIGTERM);
    wait(NULL);
    printf("[Parent] Child terminated\n");
  }
}

/* Example 5: Disk Sleep State (D) - Harder to demonstrate */
void example_disk_sleep() {
  printf("\n=== Example 5: Disk Sleep State (D) ===\n");
  printf("This state occurs during uninterruptible I/O\n");
  printf("Usually seen with slow devices or NFS\n\n");

  printf("To see 'D' state in the wild:\n");
  printf("1. Mount a slow NFS share\n");
  printf("2. Try to read from unresponsive NFS\n");
  printf("3. Use: ps aux | grep D\n\n");

  // We'll simulate with synchronous I/O
  printf("Creating file with sync I/O...\n");

  int fd = open("/tmp/test_disk_sleep", O_WRONLY | O_CREAT | O_SYNC, 0644);
  if (fd == -1) {
    perror("open");
    return;
  }

  printf("Writing with O_SYNC (should briefly enter D state)\n");
  printf("(Check quickly with: ps aux | grep %d)\n", getpid());

  // This write will be synchronous
  char buffer[1024 * 1024]; // 1MB
  memset(buffer, 'A', sizeof(buffer));

  for (int i = 0; i < 100; i++) {
    write(fd, buffer, sizeof(buffer));
  }

  close(fd);
  unlink("/tmp/test_disk_sleep");

  printf("Note: D state is very brief on fast disks\n");
  printf("More common with slow devices or network filesystems\n");
}

/* Example 6: Monitor State Transitions */
void example_state_transitions() {
  printf("\n=== Example 6: State Transitions Over Time ===\n");

  pid_t pid = fork();

  if (pid == 0) {
    // Child goes through various states
    printf("[Child %d] Starting state transitions demo\n", getpid());

    // State 1: Running (CPU work)
    print_state(getpid(), "1-Running");
    volatile long sum = 0;
    for (long i = 0; i < 50000000; i++)
      sum += i;

    // State 2: Sleeping
    print_state(getpid(), "2-Before sleep");
    sleep(2);
    print_state(getpid(), "3-After sleep");

    // State 3: Blocked on I/O
    print_state(getpid(), "4-Before I/O");
    int fd = open("/dev/urandom", O_RDONLY);
    char buf[1024];
    read(fd, buf, sizeof(buf));
    close(fd);
    print_state(getpid(), "5-After I/O");

    printf("[Child %d] All transitions complete\n", getpid());
    exit(0);
  } else {
    // Parent just waits
    wait(NULL);
    printf("[Parent] Child completed all state transitions\n");
  }
}

/* Example 7: Count processes in each state */
void example_system_state_summary() {
  printf("\n=== Example 7: System-Wide State Summary ===\n");
  printf("Scanning /proc for all process states...\n\n");

  int count_R = 0, count_S = 0, count_D = 0, count_Z = 0, count_T = 0,
      count_other = 0;

  // Scan /proc for all PIDs
  for (int pid = 1; pid < 65536; pid++) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *fp = fopen(path, "r");
    if (fp) {
      char state;
      fscanf(fp, "%*d %*s %c", &state);
      fclose(fp);

      switch (state) {
      case 'R':
        count_R++;
        break;
      case 'S':
        count_S++;
        break;
      case 'D':
        count_D++;
        break;
      case 'Z':
        count_Z++;
        break;
      case 'T':
      case 't':
        count_T++;
        break;
      default:
        count_other++;
      }
    }
  }

  printf("Process State Summary:\n");
  printf("  Running/Runnable (R): %d\n", count_R);
  printf("  Sleeping (S):         %d\n", count_S);
  printf("  Disk Sleep (D):       %d\n", count_D);
  printf("  Zombie (Z):           %d\n", count_Z);
  printf("  Stopped (T):          %d\n", count_T);
  printf("  Other:                %d\n", count_other);
  printf("  Total:                %d\n",
         count_R + count_S + count_D + count_Z + count_T + count_other);

  if (count_Z > 0) {
    printf("\n⚠️  Warning: %d zombie process(es) detected!\n", count_Z);
  }
}

int main(int argc, char *argv[]) {
  printf("=== Process States Demonstration ===\n");
  printf("PID: %d\n", getpid());
  printf("\nMonitor this program with:\n");
  printf("  ps aux | grep %d\n", getpid());
  printf("  watch -n 0.5 'ps aux | grep %d'\n\n", getpid());

  if (argc > 1) {
    int example = atoi(argv[1]);
    switch (example) {
    case 1:
      example_running();
      break;
    case 2:
      example_sleeping();
      break;
    case 3:
      example_zombie();
      break;
    case 4:
      example_stopped();
      break;
    case 5:
      example_disk_sleep();
      break;
    case 6:
      example_state_transitions();
      break;
    case 7:
      example_system_state_summary();
      break;
    default:
      printf("Usage: %s [1-7]\n", argv[0]);
      printf("  1: Running/Runnable state\n");
      printf("  2: Sleeping state\n");
      printf("  3: Zombie state\n");
      printf("  4: Stopped state\n");
      printf("  5: Disk sleep state\n");
      printf("  6: State transitions\n");
      printf("  7: System-wide summary\n");
    }
  } else {
    // Run all examples
    example_running();
    example_sleeping();
    example_zombie();
    example_stopped();
    example_disk_sleep();
    example_state_transitions();
    example_system_state_summary();
  }

  printf("\n=== All Examples Complete ===\n");
  return 0;
}

/*
 * TESTING COMMANDS:
 *
 * 1. Compile and run:
 *    gcc -o process_states 01_process_states.c
 *    ./process_states
 *
 * 2. Run specific example:
 *    ./process_states 3  # Zombie example
 *
 * 3. Monitor in another terminal:
 *    watch -n 0.5 'ps aux | grep process_states'
 *
 * 4. See all states:
 *    ps aux | awk '{print $8}' | sort | uniq -c
 *
 * EXERCISES:
 *
 * 1. Modify to create multiple zombies simultaneously
 * 2. Add example showing 'I' state (idle kernel thread)
 * 3. Create a process that cycles through all states repeatedly
 * 4. Write a monitor that tracks state changes over time
 */

