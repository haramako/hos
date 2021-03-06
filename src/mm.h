#pragma once

#include "common.h"

typedef enum {
	MM_ATTR_USER = 1,
} MemoryBlockAttribute;

typedef struct {
	uintptr_t vaddr_start;
	uintptr_t vaddr_end;
	uintptr_t *paddr;
	uint64_t page_num;
	MemoryBlockAttribute attr;
} MemoryBlock;

#define MM_BLOCK_LEN 256

typedef struct {
	MemoryBlock *blocks[MM_BLOCK_LEN];
	int block_num;
} MemoryMap;

MemoryMap *mm_new();
MemoryBlock *mm_map(MemoryMap *mm, void *vaddr, size_t page_num, MemoryBlockAttribute attr);
void mm_unmap(MemoryMap *mm, MemoryBlock *block);
MemoryBlock *mm_find_vaddr(MemoryMap *mm, void *vaddr);
void mm_print(MemoryMap *mm);
