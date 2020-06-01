#include "common.h"

#include "process.h"
#include "scheduler.h"

void syscall_exit(uint64_t *args) {
	const uint64_t exit_code = args[1];
	Process *p = scheduler_current_process();
	klog("EXIT.");
	process_exit(p, (int)exit_code);
}
