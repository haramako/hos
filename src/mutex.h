#pragma once

#include "common.h"

#include <stdatomic.h>

void spinlock_init(atomic_int *a);
void spinlock_lock(atomic_int *a);
void spinlock_unlock(atomic_int *a);
// void spinlock_init(_Atomic *a);
