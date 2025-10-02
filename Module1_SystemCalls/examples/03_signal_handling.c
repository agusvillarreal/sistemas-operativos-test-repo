/*
 * 03_signal_handling.c
 *
 * Demonstrates signal handling in Linux
 * Shows: signal(), sigaction(), kill(), alarm()
 *
 * Compile: gcc -o signal_handling 03_signal_handling.c
 * Run: ./signal_handling
 * Test: Send signals from another terminal: kill -SIGUSR1 <pid>
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Global flag - must be sig_atomic_t for signal safety */
volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t alarm_fired = 0;

/* Simple signal handler */
void simple_handler(int signum) {
  /*
   * WARNING: Only async-signal-safe functions can be called here!
   * Safe: write(), _exit(), signal operations
   * UNSAFE: printf(), malloc(), most library functions
   */
  const char msg[] = "Caught signal!\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);

  signal_count++;
}

/* More sophisticated handler using sigaction */
void detailed_handler(int signum, siginfo_t *info, void *context) {
  char buffer[256];
  int len;

  len = snprintf(buffer, sizeof(buffer),
                 "\n=== Signal Information ===\n"
                 "Signal number: %d\n"
                 "Sender PID: %d\n"
                 "Sender UID: %d\n"
                 "Signal code: %d\n"
                 "========================\n",
                 signum, info->si_pid, info->si_uid, info->si_code);

  write(STDOUT_FILENO, buffer, len);
}

/* SIGINT handler (Ctrl+C) */
void sigint_handler(int signum) {
  const char msg[] = "\nReceived SIGINT (Ctrl+C). Exiting gracefully...\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
  keep_running = 0;
}

/* SIGALRM handler */
void alarm_handler(int signum) {
  const char msg[] = "\nALARM! Timer expired!\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
  alarm_fired = 1;
}

/* SIGCHLD handler */
void sigchld_handler(int signum) {
  /* Reap zombie children */
  int status;
  pid_t pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    char buffer[128];
    int len =
        snprintf(buffer, sizeof(buffer), "[SIGCHLD] Child %d exited\n", pid);
    write(STDOUT_FILENO, buffer, len);
  }
}

/* Example 1: Basic signal handling with signal() */
void example_basic_signals() {
  printf("\n=== Example 1: Basic Signal Handling ===\n");
  printf("PID: %d\n", getpid());
  printf("Send signals from another terminal:\n");
  printf("  kill -SIGUSR1 %d\n", getpid());
  printf("  kill -SIGUSR2 %d\n", getpid());
  printf("Press Ctrl+C to exit\n\n");

  /* Register handlers */
  signal(SIGUSR1, simple_handler);
  signal(SIGUSR2, simple_handler);
  signal(SIGINT, sigint_handler);

  int counter = 0;
  while (keep_running) {
    printf("Running... %d (received %d signals)\r", counter++, signal_count);
    fflush(stdout);
    sleep(1);
  }

  printf("\nTotal signals received: %d\n", signal_count);
}

/* Example 2: Advanced signal handling with sigaction() */
void example_sigaction() {
  printf("\n=== Example 2: Advanced Signal Handling (sigaction) ===\n");
  printf("PID: %d\n", getpid());
  printf("Send: kill -SIGUSR1 %d\n", getpid());
  printf("Press Ctrl+C to exit\n");

  struct sigaction sa;

  /* Configure sigaction structure */
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = detailed_handler;
  sa.sa_flags = SA_SIGINFO; // Use sa_sigaction instead of sa_handler
  sigemptyset(&sa.sa_mask); // Don't block additional signals

  /* Register handler */
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  /* Also handle SIGINT */
  signal(SIGINT, sigint_handler);
  keep_running = 1;

  while (keep_running) {
    pause(); // Wait for signal
  }
}

/* Example 3: Sending signals between processes */
void example_kill() {
  printf("\n=== Example 3: Sending Signals (kill) ===\n");

  signal(SIGUSR1, simple_handler);

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    /* CHILD PROCESS */
    printf("[Child %d] Waiting for signal from parent...\n", getpid());
    pause(); // Wait for signal
    printf("[Child %d] Received signal! Exiting.\n", getpid());
    exit(0);
  } else {
    /* PARENT PROCESS */
    printf("[Parent %d] Child PID: %d\n", getpid(), pid);
    printf("[Parent] Sleeping 2 seconds...\n");
    sleep(2);

    printf("[Parent] Sending SIGUSR1 to child...\n");
    if (kill(pid, SIGUSR1) == -1) {
      perror("kill");
      exit(EXIT_FAILURE);
    }

    int status;
    wait(&status);
    printf("[Parent] Child terminated\n");
  }
}

/* Example 4: Alarm timer */
void example_alarm() {
  printf("\n=== Example 4: Alarm Timer ===\n");

  signal(SIGALRM, alarm_handler);

  printf("Setting alarm for 3 seconds...\n");
  alarm(3);

  printf("Doing work...\n");
  for (int i = 0; i < 10 && !alarm_fired; i++) {
    printf("Working... %d\n", i);
    sleep(1);
  }

  if (alarm_fired) {
    printf("Alarm interrupted our work!\n");
  }

  /* Cancel any remaining alarm */
  alarm(0);
}

/* Example 5: Signal masking (blocking) */
void example_signal_masking() {
  printf("\n=== Example 5: Signal Masking ===\n");
  printf("PID: %d\n", getpid());

  signal(SIGUSR1, simple_handler);

  sigset_t set, oldset;

  /* Block SIGUSR1 */
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);

  printf("Blocking SIGUSR1...\n");
  if (sigprocmask(SIG_BLOCK, &set, &oldset) == -1) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  printf("SIGUSR1 is now blocked.\n");
  printf("Send signals (they will be queued): kill -SIGUSR1 %d\n", getpid());

  for (int i = 3; i > 0; i--) {
    printf("Blocked for %d more seconds...\n", i);
    sleep(1);
  }

  printf("\nUnblocking SIGUSR1...\n");
  if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  printf("Signal delivered! Count: %d\n", signal_count);
  sleep(1);
}

/* Example 6: SIGCHLD for zombie prevention */
void example_sigchld() {
  printf("\n=== Example 6: SIGCHLD Handler (Zombie Prevention) ===\n");

  /* Setup SIGCHLD handler */
  signal(SIGCHLD, sigchld_handler);

  printf("[Parent %d] Creating 3 children...\n", getpid());

  for (int i = 0; i < 3; i++) {
    pid_t pid = fork();

    if (pid == 0) {
      /* CHILD */
      printf("[Child %d] Started, will exit in %d seconds\n", getpid(), i + 1);
      sleep(i + 1);
      printf("[Child %d] Exiting\n", getpid());
      exit(i);
    }
  }

  /* Parent does other work (children auto-reaped by SIGCHLD handler) */
  printf("[Parent] Doing other work while children run...\n");

  for (int i = 0; i < 5; i++) {
    printf("[Parent] Working... %d\n", i);
    sleep(1);
  }

  printf("[Parent] Done. All children auto-reaped.\n");
}

/* Example 7: Signal safety demonstration */
void unsafe_handler(int signum) {
  /* THIS IS UNSAFE! printf() is not async-signal-safe */
  printf("This is unsafe in a signal handler!\n"); // DON'T DO THIS

  /* Use write() instead: */
  const char msg[] = "This is safe!\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

void example_signal_safety() {
  printf("\n=== Example 7: Signal Safety ===\n");
  printf("Demonstrating unsafe vs safe signal handlers\n");

  signal(SIGUSR1, unsafe_handler);

  printf("PID: %d\n", getpid());
  printf("Send: kill -SIGUSR1 %d\n", getpid());
  printf("Waiting 5 seconds...\n");

  sleep(5);

  printf("\nNote: printf() in signal handler can cause deadlock\n");
  printf("if the signal interrupts printf() in main program!\n");
}

int main(int argc, char *argv[]) {
  printf("=== Signal Handling Demonstration ===\n");

  /* Prevent zombie processes */
  signal(SIGCHLD, SIG_IGN); // Ignore child termination

  if (argc > 1) {
    int example = atoi(argv[1]);
    switch (example) {
    case 1:
      example_basic_signals();
      break;
    case 2:
      example_sigaction();
      break;
    case 3:
      example_kill();
      break;
    case 4:
      example_alarm();
      break;
    case 5:
      example_signal_masking();
      break;
    case 6:
      example_sigchld();
      break;
    case 7:
      example_signal_safety();
      break;
    default:
      printf("Usage: %s [1-7]\n", argv[0]);
      printf("  1: Basic signal handling\n");
      printf("  2: sigaction() advanced handling\n");
      printf("  3: kill() - sending signals\n");
      printf("  4: alarm() timer\n");
      printf("  5: Signal masking/blocking\n");
      printf("  6: SIGCHLD handler\n");
      printf("  7: Signal safety\n");
    }
  } else {
    printf("Run with argument 1-7 to see specific examples\n");
    printf("Example: %s 1\n", argv[0]);
  }

  return EXIT_SUCCESS;
}

/*
 * COMMON SIGNALS:
 *
 * SIGHUP    1  Hangup
 * SIGINT    2  Interrupt (Ctrl+C)
 * SIGQUIT   3  Quit (Ctrl+\)
 * SIGILL    4  Illegal instruction
 * SIGABRT   6  Abort
 * SIGFPE    8  Floating-point exception
 * SIGKILL   9  Kill (cannot be caught!)
 * SIGSEGV  11  Segmentation fault
 * SIGPIPE  13  Broken pipe
 * SIGALRM  14  Alarm clock
 * SIGTERM  15  Termination request
 * SIGCHLD  17  Child process terminated
 * SIGCONT  18  Continue if stopped
 * SIGSTOP  19  Stop (cannot be caught!)
 * SIGUSR1  10  User-defined signal 1
 * SIGUSR2  12  User-defined signal 2
 *
 * TESTING COMMANDS:
 *
 * 1. Compile:
 *    gcc -o signal_handling 03_signal_handling.c
 *
 * 2. Run example 1 and send signals from another terminal:
 *    Terminal 1: ./signal_handling 1
 *    Terminal 2: kill -SIGUSR1 $(pgrep signal_handling)
 *
 * 3. Trace signals:
 *    strace -e trace=signal ./signal_handling 3
 *
 * 4. Test with multiple signals rapidly:
 *    for i in {1..10}; do kill -SIGUSR1 $(pgrep signal_handling); done
 *
 * EXERCISES:
 *
 * 1. Implement a program that counts down from 10 using SIGALRM
 *
 * 2. Create a daemon process that handles SIGHUP to reload configuration
 *
 * 3. Implement graceful shutdown: SIGTERM = clean exit, SIGINT = immediate exit
 *
 * 4. Write a program that catches SIGSEGV and prints a custom error message
 *    (Note: This is tricky and generally not recommended in production!)
 */
