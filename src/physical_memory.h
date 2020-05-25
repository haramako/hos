#pragma once

#include "common.h"

#include "efi/memory_map.h"

struct EFI_MemoryMap_;

#define PHYSICAL_MEMORY_BLOCK_LEN 128

typedef struct FreeBlock_ {
	uintptr_t start;
	uintptr_t pages;
} FreeBlock;

typedef struct PhysicalMemory_ {
	int block_len;
	FreeBlock blocks[PHYSICAL_MEMORY_BLOCK_LEN];
} PhysicalMemory;

PhysicalMemory *physical_memory_instance();
void physical_memory_init(struct EFI_MemoryMap_ *memory_map);
uintptr_t physical_memory_alloc(uintptr_t pages);

uintptr_t physmem_v2p(void *addr);
