#include <stdlib.h>
#include <unistd.h>

int main() {
  pid_t pid = fork();

  if (pid == 0) {
    // Child exits immediately
    exit(0);
  }

  // Parent doesn't call wait() - CREATES ZOMBIE
  sleep(60); // Child is zombie for 60 seconds

  return 0;
}