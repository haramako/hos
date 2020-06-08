#include <sys/types.h>

// int kill(int pid, int sig) { return 0; }

pid_t waitpid(int pid, int *status, int options) { return 0; }

pid_t fork() { return 0; }

int execvp(const char *filename, char *const argv[], char *const envp[]) { return 0; }

uid_t getuid(void) { return 0; }

int gethostname(char *name, size_t len) { return 0; }
