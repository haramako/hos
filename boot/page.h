#pragma once

#include "common.h"

typedef union PACKED PageMapEntry_ {
	uint64_t raw;
	struct {
		int present : 1;
		int is_read : 1;
		int is_user : 1;
		int write_through : 1;
		int cache_disabled : 1;
		int accessed : 1;
		int dummy0 : 1;
		int page_size : 1;
		int ignored : 1;
		int available : 3;
		// int addr : 32;
		// int dummy1 : 16;
	} x;
} PageMapEntry;

#define PAGE_MAP_TABLE_LEN 512
#define PAGE_SIZE 4096

#define canonical_addr(addr) (((addr) & (1ULL << 47)) ? ((addr) | 0xffff000000000000) : (addr))

PageMapEntry *page_current_pml4();

typedef bool (*Page_FindEntryCallback)(int level, PageMapEntry *entry, void *data);
PageMapEntry *page_find_entry(PageMapEntry *pml, int level, uint64_t vaddr, Page_FindEntryCallback callback,
							  void *callback_data);
void page_map_addr(uintptr_t paddr, uintptr_t vaddr, int num_page);
