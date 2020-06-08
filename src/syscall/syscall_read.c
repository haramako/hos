#include "common.h"

#include "fs.h"
#include "pipe.h"
#include "process.h"
#include "scheduler.h"

uint64_t syscall_read(uint64_t *args) {
	int fd_num = (int)args[1];
	char *buf = (char *)args[2];
	size_t size = args[3];

	klog("syscall_read %d %p %ld", fd_num, buf, size);

	Process *proc = scheduler_current_process();
	FileDescriptor *fd = &proc->fds[fd_num];

	switch (fd->type) {
	case FD_TYPE_INODE: {

		kcheck0(fd->inode);

		size_t rest = fd->inode->file.size - fd->pos;
		if (size > rest) {
			size = rest;
		}

		error_t err = fs_read(fd->inode, buf, fd->pos, &size);
		TRY(err);

		fd->pos += size;

		return size;
	}
	case FD_TYPE_PIPE: {
		error_t err = pipe_read(fd->pipe, buf, size);
		TRY(err);

		return size;
	}
	case FD_TYPE_CONSOLE: {
		buf[0] = 'A';
		return 1;
	}
	default:
		kpanic("Invalid fd type %d.", fd->type);
	}
}
