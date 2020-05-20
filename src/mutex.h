#pragma once

#include "common.h"

#include <stdatomic.h>

#include "asm.h"

inline void spinlock_init(atomic_int *a) { *a = 0; }

inline void spinlock_lock(atomic_int *a) {
	for (;;) {
		atomic_int zero = 0;
		if (atomic_compare_exchange_strong(a, (void *)&zero, 1)) break;
		asm_pause();
	}
}

inline bool spinlock_trylock(atomic_int *a, int limit) {
	atomic_int zero = 0;
	return atomic_compare_exchange_strong(a, (void *)&zero, 1);
}

inline bool spinlock_trylock_with_limit(atomic_int *a, int limit) {
	for (int i = 0; i < limit; i++) {
		atomic_int zero = 0;
		if (atomic_compare_exchange_strong(a, (void *)&zero, 1)) return true;
		asm_pause();
	}
	return false;
}

inline void spinlock_unlock(atomic_int *a) {
	assert(*a != 0);
	atomic_store(a, 0);
}

typedef struct {
	atomic_int lock;
	struct Process_ *wait;
} Mutex;

void mutex_init(Mutex *m);
error_t mutex_lock(Mutex *m);
error_t mutex_unlock(Mutex *m);
