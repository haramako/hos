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
		uint8_t available : 3;
		uint64_t addr : 36;
		uint64_t dummy1 : 16;
	} x;
} PageMapEntry;

#define PAGE_MAP_TABLE_LEN 512

typedef struct {
	uint64_t mask;
	bool show_nonleaf;
	bool show_fixed;
} PMEDisplayConfig;

inline uint64_t canonical_addr(uint64_t addr) {
	if (addr & (1UL << 47)) {
		return addr | 0xffff000000000000;
	} else {
		return addr;
	}
}

inline uint64_t pme_addr(PageMapEntry pme) { return canonical_addr(pme.x.addr << 12); }
inline void pme_set_addr(PageMapEntry *pme, uint64_t paddr) { pme->x.addr = paddr >> 12; }

const char *pme_flags(PageMapEntry pml);
void page_map_entry_print(PageMapEntry *pml4);

void page_init(PageMapEntry *pml4);
uintptr_t page_v2p(PageMapEntry *pml4, void *addr);
PageMapEntry *page_copy_page_map_table(PageMapEntry *pml4);
void page_alloc_addr(void *addr, int num_page);
void page_alloc_addr_prelude(void *addr, int num_page);