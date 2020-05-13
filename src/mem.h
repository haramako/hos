#pragma once

#include "common.h"

#include "mm.h"
#include "page.h"

MemoryMap *g_kernel_mm;

void mem_init();
uintptr_t mem_sbrk(int diff);
error_t mem_alloc_addr(PageMapEntry *pml4, MemoryMap *mm, void *vaddr);
error_t mem_alloc_memory_block(PageMapEntry *pml4, MemoryBlock *mm);
