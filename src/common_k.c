#include "common_k.h"

void kpanic(const char *msg) {
	klog(msg);
	while (1) {
	}
}

const char *humanize_size(uint64_t size) {
	static char buf[16];
	if (size >= (1LLU << 30)) {
		snprintf(buf, sizeof(buf), "%4ld GB", size >> 30);
	} else if (size >= (1LLU << 20)) {
		snprintf(buf, sizeof(buf), "%4ld MB", size >> 20);
	} else if (size >= (1LLU << 10)) {
		snprintf(buf, sizeof(buf), "%4ld KB", size >> 10);
	} else {
		snprintf(buf, sizeof(buf), "%ld", size);
	}
	return buf;
}
