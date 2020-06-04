#include "common.h"

#include "console.h"

uint64_t syscall_write(uint64_t *args) {
	const uint64_t fd = args[1];
	const char *buf = (char *)args[2];
	uint64_t nbyte = args[3];

	// klog("write %ld, %p, %ld", fd, buf, nbyte);

	if (fd != 1) {
		kpanic("Only stdout is supported for now.");
	}
	console_write(buf, nbyte);

	return 0;
}
