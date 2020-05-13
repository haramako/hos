#pragma once

#include "common.h"
#include "mm.h"

MemoryMap *g_kernel_mm;

void mem_init();
uintptr_t mem_sbrk(int diff);
