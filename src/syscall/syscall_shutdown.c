#include "common.h"

void syscall_shutdown(uint64_t *args) {
	kinfo("Shutdown.");
	kshutdown();
}
