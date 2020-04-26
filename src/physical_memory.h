#pragma once

#include "common.h"
#include "efi.h"

#define PHYSICAL_MEMORY_BLOCK_LEN 128

typedef struct FreeBlock {
	uintptr_t start;
	uintptr_t pages;
} FreeBlock;

typedef struct PhysicalMemory {
	int block_len;
	FreeBlock blocks[PHYSICAL_MEMORY_BLOCK_LEN];
} PhysicalMemory;

PhysicalMemory* physical_memory_instance();
void physical_memory_init(EFI_MemoryMap *memory_map);
uintptr_t physical_memory_alloc(uintptr_t pages);
