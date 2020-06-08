#include "common.h"

#include "console.h"
#include "pipe.h"
#include "process.h"
#include "scheduler.h"

uint64_t syscall_write(uint64_t *args) {
	const uint64_t fd_num = args[1];
	const char *buf = (char *)args[2];
	uint64_t size = args[3];

	ktrace("write %ld, %p, %ld", fd_num, buf, size);

	Process *proc = scheduler_current_process();
	FileDescriptor *fd = &proc->fds[fd_num];

#if 0
	if (fd != 1) {
		kpanic("Only stdout is supported for now.");
	}
#endif

	switch (fd->type) {
	case FD_TYPE_INODE: {
		// console_write(buf, size);
		return 0;
	} break;
	case FD_TYPE_PIPE: {
		error_t err = pipe_write(fd->pipe, buf, size);
		TRY(err);

		return size;
	}
	case FD_TYPE_CONSOLE: {
		console_write(buf, size);
		return 0;
	}
	default:
		kpanic("Invalid fd type %d.", fd->type);
	}
}
