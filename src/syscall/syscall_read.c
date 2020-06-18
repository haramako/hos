#include "common.h"

#include "fs.h"
#include "pipe.h"
#include "process.h"
#include "serial.h"

void hoge_(void *data) { Sleep(); }

uint64_t syscall_read(uint64_t *args) {
	int fd_num = (int)args[1];
	char *buf = (char *)args[2];
	size_t size = args[3];

	ktrace("syscall_read %d %p %ld", fd_num, buf, size);

	Process *proc = process_current();
	FileDescriptor *fd = &proc->fds[fd_num];

	switch_rsp(proc->ctx->kernel_rsp, hoge_, NULL);

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
		for (;;) {
			char c = serial_read_char_received(serial_get_port(1));

			if (c == 0) {
				switch_rsp(proc->ctx->kernel_rsp, hoge_, NULL);
				continue;
			}

			buf[0] = c;
			return 1;
		}
	}
	default:
		kpanic("Invalid fd type %d.", fd->type);
	}
}
