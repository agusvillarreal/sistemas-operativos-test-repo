/*
 * 02_proc_reader.c
 *
 * Demonstrates reading process information from /proc filesystem
 * Shows how to parse /proc/[pid]/stat and /proc/[pid]/status
 *
 * Compile: gcc -o proc_reader 02_proc_reader.c
 * Run: ./proc_reader [pid]
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int pid;
  char comm[256];
  char state;
  int ppid;
  int pgrp;
  unsigned long utime;
  unsigned long stime;
  long priority;
  long nice;
  unsigned long vsize;
  long rss;
} proc_stat_t;

/* Read /proc/[pid]/stat */
int read_proc_stat(int pid, proc_stat_t *stat) {
  char path[256];
  snprintf(path, sizeof(path), "/proc/%d/stat", pid);

  FILE *fp = fopen(path, "r");
  if (!fp)
    return -1;

  int ret =
      fscanf(fp,
             "%d %s %c %d %d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d "
             "%ld %ld %*d %*d %*u %lu %ld",
             &stat->pid, stat->comm, &stat->state, &stat->ppid, &stat->pgrp,
             &stat->utime, &stat->stime, &stat->priority, &stat->nice,
             &stat->vsize, &stat->rss);

  fclose(fp);
  return (ret >= 8) ? 0 : -1;
}

/* Read memory info from /proc/[pid]/status */
void read_memory_info(int pid) {
  char path[256], line[256];
  snprintf(path, sizeof(path), "/proc/%d/status", pid);

  FILE *fp = fopen(path, "r");
  if (!fp)
    return;

  printf("\n=== Memory Information ===\n");

  while (fgets(line, sizeof(line), fp)) {
    if (strncmp(line, "Vm", 2) == 0 || strncmp(line, "Rss", 3) == 0) {
      printf("%s", line);
    }
  }

  fclose(fp);
}

/* Read open file descriptors */
void read_open_files(int pid) {
  char path[256];
  snprintf(path, sizeof(path), "/proc/%d/fd", pid);

  DIR *dir = opendir(path);
  if (!dir) {
    printf("Cannot read file descriptors\n");
    return;
  }

  printf("\n=== Open File Descriptors ===\n");

  struct dirent *entry;
  int count = 0;

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] == '.')
      continue;

    char fd_path[512], link_target[512];
    snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%s", pid, entry->d_name);

    ssize_t len = readlink(fd_path, link_target, sizeof(link_target) - 1);
    if (len != -1) {
      link_target[len] = '\0';
      printf("  FD %s -> %s\n", entry->d_name, link_target);
      count++;
    }
  }

  printf("Total open files: %d\n", count);
  closedir(dir);
}

/* Read command line */
void read_cmdline(int pid) {
  char path[256];
  snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

  FILE *fp = fopen(path, "r");
  if (!fp)
    return;

  printf("\n=== Command Line ===\n");
  char c;
  while ((c = fgetc(fp)) != EOF) {
    if (c == '\0')
      putchar(' ');
    else
      putchar(c);
  }
  printf("\n");

  fclose(fp);
}

/* Print detailed process information */
void print_process_info(int pid) {
  proc_stat_t stat;

  if (read_proc_stat(pid, &stat) != 0) {
    printf("Error: Cannot read process %d\n", pid);
    return;
  }

  printf("\n=== Process Information ===\n");
  printf("PID:           %d\n", stat.pid);
  printf("Command:       %s\n", stat.comm);
  printf("State:         %c (%s)\n", stat.state,
         stat.state == 'R'   ? "Running"
         : stat.state == 'S' ? "Sleeping"
         : stat.state == 'D' ? "Disk Sleep"
         : stat.state == 'Z' ? "Zombie"
         : stat.state == 'T' ? "Stopped"
                             : "Unknown");
  printf("Parent PID:    %d\n", stat.ppid);
  printf("Process Group: %d\n", stat.pgrp);
  printf("User Time:     %lu ticks\n", stat.utime);
  printf("System Time:   %lu ticks\n", stat.stime);
  printf("Priority:      %ld\n", stat.priority);
  printf("Nice:          %ld\n", stat.nice);
  printf("Virtual Size:  %lu bytes (%.2f MB)\n", stat.vsize,
         stat.vsize / (1024.0 * 1024.0));
  printf("Resident Set:  %ld pages (%.2f MB)\n", stat.rss,
         (stat.rss * sysconf(_SC_PAGESIZE)) / (1024.0 * 1024.0));

  read_memory_info(pid);
  read_cmdline(pid);
  read_open_files(pid);
}

/* List all processes */
void list_all_processes() {
  printf("\n=== All Processes ===\n");
  printf("%-7s %-7s %-1s %-20s %10s %10s\n", "PID", "PPID", "S", "COMMAND",
         "VSIZE", "RSS");
  printf("%s\n", "---------------------------------------------------------------"
                 "-----------");

  DIR *dir = opendir("/proc");
  if (!dir) {
    perror("opendir /proc");
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    // Check if directory name is a number (PID)
    if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
      continue;

    int pid = atoi(entry->d_name);
    proc_stat_t stat;

    if (read_proc_stat(pid, &stat) == 0) {
      // Remove parentheses from comm
      char comm[256];
      strncpy(comm, stat.comm, sizeof(comm));
      if (comm[0] == '(') {
        size_t len = strlen(comm);
        if (len > 2 && comm[len - 1] == ')') {
          memmove(comm, comm + 1, len - 1);
          comm[len - 2] = '\0';
        }
      }

      printf("%-7d %-7d %c %-20s %10lu %10ld\n", stat.pid, stat.ppid,
             stat.state, comm, stat.vsize / 1024, // KB
             stat.rss * sysconf(_SC_PAGESIZE) / 1024); // KB
    }
  }

  closedir(dir);
}

/* Find zombies */
void find_zombies() {
  printf("\n=== Zombie Processes ===\n");

  DIR *dir = opendir("/proc");
  if (!dir) {
    perror("opendir /proc");
    return;
  }

  int zombie_count = 0;
  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
      continue;

    int pid = atoi(entry->d_name);
    proc_stat_t stat;

    if (read_proc_stat(pid, &stat) == 0 && stat.state == 'Z') {
      printf("Zombie found: PID %d (%s), Parent PID %d\n", stat.pid, stat.comm,
             stat.ppid);
      zombie_count++;
    }
  }

  if (zombie_count == 0) {
    printf("No zombie processes found.\n");
  } else {
    printf("\nTotal zombies: %d\n", zombie_count);
  }

  closedir(dir);
}

/* Process tree (simplified) */
void print_process_tree(int pid, int level) {
  proc_stat_t stat;
  if (read_proc_stat(pid, &stat) != 0)
    return;

  // Print indentation
  for (int i = 0; i < level; i++)
    printf("  ");

  printf("├─ [%d] %s (state: %c)\n", stat.pid, stat.comm, stat.state);

  // Find children (scan all processes for this ppid)
  DIR *dir = opendir("/proc");
  if (!dir)
    return;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
      continue;

    int child_pid = atoi(entry->d_name);
    proc_stat_t child_stat;

    if (read_proc_stat(child_pid, &child_stat) == 0 &&
        child_stat.ppid == pid) {
      print_process_tree(child_pid, level + 1);
    }
  }

  closedir(dir);
}

int main(int argc, char *argv[]) {
  printf("=== /proc Filesystem Reader ===\n");

  if (argc == 1) {
    printf("Usage: %s <command> [args]\n\n", argv[0]);
    printf("Commands:\n");
    printf("  info <pid>    - Detailed info about process\n");
    printf("  list          - List all processes\n");
    printf("  zombies       - Find zombie processes\n");
    printf("  tree <pid>    - Show process tree from pid\n");
    printf("  self          - Show info about this process\n");
    return 0;
  }

  if (strcmp(argv[1], "info") == 0) {
    if (argc < 3) {
      printf("Usage: %s info <pid>\n", argv[0]);
      return 1;
    }
    print_process_info(atoi(argv[2]));
  } else if (strcmp(argv[1], "list") == 0) {
    list_all_processes();
  } else if (strcmp(argv[1], "zombies") == 0) {
    find_zombies();
  } else if (strcmp(argv[1], "tree") == 0) {
    if (argc < 3) {
      printf("Usage: %s tree <pid>\n", argv[0]);
      return 1;
    }
    printf("\n=== Process Tree from PID %s ===\n", argv[2]);
    print_process_tree(atoi(argv[2]), 0);
  } else if (strcmp(argv[1], "self") == 0) {
    print_process_info(getpid());
  } else {
    printf("Unknown command: %s\n", argv[1]);
    return 1;
  }

  return 0;
}

/*
 * USAGE EXAMPLES:
 *
 * # Get info about init process
 * ./proc_reader info 1
 *
 * # List all processes
 * ./proc_reader list
 *
 * # Find zombies
 * ./proc_reader zombies
 *
 * # Show process tree
 * ./proc_reader tree 1
 *
 * # Info about this program
 * ./proc_reader self
 */

