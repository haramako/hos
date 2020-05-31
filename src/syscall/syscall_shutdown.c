#include "common.h"

void syscall_shutdown(uint64_t *args) {
	ktrace("Shutdown.");
	kshutdown();
}
