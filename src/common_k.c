#include "common_k.h"

void kpanic(const char *msg) {
	klog(msg);
	while (1) {
	}
}
