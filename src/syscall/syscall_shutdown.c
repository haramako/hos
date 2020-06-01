#include "common.h"

uint64_t syscall_shutdown(uint64_t *args) {
	kinfo("Shutdown.");
	kshutdown();
	return 0;
}
