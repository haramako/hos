#include <stddef.h>
#include <stdio.h>
#include <syscall_func.h>

int open(const char *__file, int __oflag, ...) { return __open(__file, __oflag); }

_READ_WRITE_RETURN_TYPE write(int __fd, const void *__buf, size_t __nbyte) {
	__write(1, __buf, __nbyte);
	return __nbyte;
}

_READ_WRITE_RETURN_TYPE read(int __fd, void *__buf, size_t __nbytes) { return __read(__fd, __buf, __nbytes); }

int chdir(const char *path) { return 0; }

int pipe(int pipefd[2]) { return 0; }

int dup2(int oldfd, int newfd) { return 0; }
