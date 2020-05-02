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
		int dummy0 : 1;
		int page_size : 1;
		int ignored : 1;
		int available : 3;
		uint64_t addr : 36;
		uint64_t dummy1 : 16;
	} x;
} PageMapEntry;

#define PAGE_MAP_TABLE_LEN 512

typedef struct {
	uint64_t mask;
} PMEDisplayConfig;

inline uint64_t canonical_addr(uint64_t addr) {
	if (addr & (1UL << 47)) {
		return addr | 0xffff000000000000;
	} else {
		return addr;
	}
}

inline uint64_t pme_addr(PageMapEntry pml) { return canonical_addr(pml.x.addr << 12); }

const char *pme_flags(PageMapEntry pml);
void page_map_entry_print(PageMapEntry *pml4);
