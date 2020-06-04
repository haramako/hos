#include "common.h"

#include "fs.h"
#include "process.h"
#include "scheduler.h"

#define RETURN_IF_ERROR(err) \
	if ((err) != ERR_OK) { \
		return (err); \
	}

uint64_t syscall_read(uint64_t *args) {
	int fd_num = (int)args[1];
	char *buf = (char *)args[2];
	size_t size = args[3];

	ktrace("syscall_read %d %p %ld", fd_num, buf, size);

	Process *proc = scheduler_current_process();
	FileDescriptor *fd = &proc->fds[fd_num];

	kcheck0(fd->inode);

	size_t rest = fd->inode->file.size - fd->pos;
	if (size > rest) {
		size = rest;
	}

	error_t err = fs_read(fd->inode, buf, fd->pos, &size);
	RETURN_IF_ERROR(err);

	fd->pos += size;

	return size;
}
