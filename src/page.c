#include "page.h"

#include "asm.h"
#include "interrupt.h"
#include "mem.h"
#include "mm.h"
#include "physical_memory.h"

extern inline uint64_t canonical_addr(uint64_t addr);
extern inline uint64_t pme_addr(PageMapEntry pme);
extern inline void pme_set_addr(PageMapEntry *pme, uint64_t paddr);
extern inline uint64_t pme_flag(PageMapEntry p);
extern inline void *page_align(void *addr);
extern inline bool pme_is_leaf_(PageMapEntry pme, int level);

static PageMapEntry *get_pml4_() { return (PageMapEntry *)ReadCR3(); }

PageMapEntry *page_find_entry(PageMapEntry *pml, int level, uint64_t vaddr, Page_FindEntryCallback callback,
							  void *callback_data) {
	int shift = (12 + (level - 1) * 9);
	uint64_t entry_num = (vaddr >> shift) & (PAGE_MAP_TABLE_LEN - 1);
	PageMapEntry *pme = &pml[entry_num];
	// klog("%d %p %d %p", level, opt->vaddr, entry_num, pme);

	if (!pme->x.present) {
		if (callback) {
			if (!callback(level - 1, pme, callback_data)) return NULL;
		} else {
			return NULL;
		}
	}

	bool is_leaf = pme_is_leaf_(*pme, level);

	if (is_leaf) {
		return pme;
	} else {
		return page_find_entry((PageMapEntry *)(pme_addr(*pme)), level - 1, vaddr, callback, callback_data);
	}
}

uintptr_t page_v2p(PageMapEntry *pml4, void *addr) {
	PageMapEntry *pme = page_find_entry(pml4, 4, (uint64_t)addr, NULL, NULL);
	if (pme) {
		return pme_addr(*pme);
	} else {
		return 0;
	}
}

PageMapEntry *copy_page_map_table_(PageMapEntry *pml, int level) {
	PageMapEntry *new_pml = (PageMapEntry *)physical_memory_alloc(1);
	kcheck(new_pml, "Can't allocate new page map table.");
	memset(new_pml, 0, 4096);

	int num_copied = 0;
	int num_fixed = 0;
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry pme = pml[i];
		if (!pme.x.present) continue;
		num_copied++;
		if (pme_is_leaf_(pme, level)) {
			new_pml[i] = pme;
		} else {
			PageMapEntry *child = copy_page_map_table_((PageMapEntry *)pme_addr(pme), level - 1);
			new_pml[i] = pme;
			pme_set_addr(&new_pml[i], (uint64_t)child);
		}
	}
	ktrace("Copy page map table %018p level %d, copy %d entries, fixed %d entries", pml, level, num_copied, num_fixed);
	return new_pml;
}

PageMapEntry *page_copy_page_map_table(PageMapEntry *pml4) { return copy_page_map_table_(pml4, 4); }

void page_init() {}

static bool alloc_page_callback2_(int level, PageMapEntry *pme, void *data) {
	uintptr_t page = physical_memory_alloc(1);
	memset((void *)page, 0, PAGE_SIZE);
	pme_set_addr(pme, page);
	pme->x.present = 1;
	pme->x.is_read = 1;
	pme->x.is_user = 1;
	// klog("alloc_page lv=%d, pme=%p(%p), addr=%p", level, pme, pme->raw, data);
	return true;
}

void page_pme_alloc_addr(PageMapEntry *pml4, void *addr, int num_page, bool alloc, bool is_user) {
	kcheck((((uintptr_t)addr) & (PAGE_SIZE - 1)) == 0, "Invalid addr. addr must aligned page size.");

	for (int i = 0; i < num_page; i++) {
		uintptr_t target_addr = (uintptr_t)addr + PAGE_SIZE * i;
		page_find_entry(pml4, 4, target_addr, alloc_page_callback2_, (void *)target_addr);
	}
}

void page_alloc_addr(void *addr, int num_page, bool alloc, bool is_user) {
	page_pme_alloc_addr(get_pml4_(), addr, num_page, alloc, is_user);
}

void page_memcpy(PageMapEntry *dest_pml4, void *dest, void *src, size_t size) {
	for (size_t i = 0; i < size; i++) {
		uintptr_t addr_rest = ((uintptr_t)dest + i) & (PAGE_SIZE - 1);
		PageMapEntry *pme = page_find_entry(dest_pml4, 4, (uint64_t)page_align((uint8_t *)dest + i), NULL, NULL);
		kcheck(pme, "Page not mapped");
		uint8_t *a = (uint8_t *)pme_addr(*pme) + addr_rest;
		// klog("%lld, %p", i, a);
		*a = ((uint8_t *)src)[i];
	}
}

PageMapEntry *page_current_pml4() { return get_pml4_(); }
