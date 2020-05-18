#include "mutex.h"

void spinlock_init(atomic_int *a) { *a = 0; }

void spinlock_lock(atomic_int *a) {
	for (;;) {
		atomic_int zero = 0;
		if (atomic_compare_exchange_strong(a, (void *)&zero, 1)) break;
	}
}

void spinlock_unlock(atomic_int *a) {
	for (;;) {
		atomic_int one = 1;
		if (atomic_compare_exchange_strong(a, (void *)&one, 0)) break;
	}
}
