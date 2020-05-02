#pragma once

#include "common.h"

typedef union PACKED PageMapEntry {
	uint64_t raw;
	struct {
		int present : 1;
		int is_read : 1;
		int is_user : 1;
		int write_through : 1;
		int cache_disabled : 1;
		int accessed : 1;
		int dummy : 1;
		int page_size : 1;
		int ignored : 1;
		int available : 3;
		uint64_t addr : 52;
	} x;
} PageMapEntry;

#define PAGE_MAP_TABLE_LEN 512

typedef struct PACKED PageMapTable {
	PageMapEntry entries[PAGE_MAP_TABLE_LEN];
} PageMapTable;

inline uint64_t pme_addr(PageMapEntry pml) { return pml.x.addr << 12; }

const char *pme_flags(PageMapEntry pml);
void page_map_entry_print(PageMapEntry *pml4);
