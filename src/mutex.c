#include "mutex.h"

#include "asm.h"

extern inline void spinlock_init(atomic_int *a);
extern inline void spinlock_lock(atomic_int *a);
extern inline bool spinlock_trylock(atomic_int *a, int limit);
extern inline bool spinlock_trylock_with_limit(atomic_int *a, int limit);
extern inline void spinlock_unlock(atomic_int *a);

void mutex_init(Mutex *m) { m = memset(m, 0, sizeof(Mutex)); }

error_t mutex_lock(Mutex *m) {
#if 0
	if( spinlock_trylock(m->a, 1000) ){
		return ERR_OK;
	}else{
		return ERR_OK;
	}
#else
	return ERR_OK;
#endif
}

error_t mutex_unlock(Mutex *m) { return ERR_OK; }
