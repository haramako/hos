#include "common.h"

#include "process.h"

uint64_t syscall_exit(uint64_t *args) {
	const uint64_t exit_code = args[1];
	Process *p = process_current();
	klog("EXIT.");
	process_exit(p, (int)exit_code);
	return 0;
}
