#include "common.h"

#include "fs.h"

void syscall_open(uint64_t *args) {
	char *path = (char *)args[1];
	int flag = (int)args[2];

	Process *proc = scheduler_current_process();
	int fd = proc->fd_num++;
	memset(proc->fds[fd], 0, sizeof(FileDescriptor));

	return fd;
}
