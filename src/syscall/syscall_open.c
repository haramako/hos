#include "common.h"

#include "fs.h"
#include "process.h"

uint64_t syscall_open(uint64_t *args) {
	char *path = (char *)args[1];
	int flag = (int)args[2];

	ktrace("syscall_open %s %d", path, flag);

	Process *proc = process_current();
	int fd_num = proc->fd_num++;
	FileDescriptor *fd = &proc->fds[fd_num];
	memset(fd, 0, sizeof(FileDescriptor));

	INode *inode;
	error_t err = fs_open(path, flag, &inode);
	if (err != ERR_OK) return -err;
	kcheck_ok(err);
	kcheck0(inode);

	fd->type = FD_TYPE_INODE;
	fd->inode = inode;

	return fd_num;
}
