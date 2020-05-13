#pragma once

#include "common.h"

#include "page.h"

typedef struct {
	bool is_user;
} PageAttribute;

typedef struct {
	uintptr_t vaddr_start;
	uintptr_t vaddr_end;
	uintptr_t *paddr;
	uint64_t page_num;
	PageAttribute attr;
} MemoryBlock;

#define MM_BLOCK_LEN 256

typedef struct {
	MemoryBlock *blocks[MM_BLOCK_LEN];
	int block_num;
	PageMapEntry *pml4;
} MemoryMap;

MemoryMap *mm_new();
MemoryBlock *mm_alloc(MemoryMap *mm, void *vaddr, int page_num, const PageAttribute *attr);
void mm_free(MemoryMap *mm, MemoryBlock *block);
MemoryBlock *mm_find_vaddr(MemoryMap *mm, void *vaddr);
void mm_print(MemoryMap *mm);
