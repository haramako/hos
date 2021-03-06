#include "page.h"

#include "asm.h"
#include "console.h"
#include "efi_util.h"
#include "util.h"

static uint64_t pme_addr(PageMapEntry pme) { return canonical_addr(pme.raw & 0x0000FFFFFFFFF000); }
static void pme_set_addr(PageMapEntry *pme, uint64_t paddr) {
	pme->raw = (pme->raw & 0xFFF) | (paddr & 0x0000FFFFFFFFF000);
}

static bool pme_is_leaf(PageMapEntry pme, int level) {
	switch (level) {
	case 1:
		return true;
	case 2:
		return pme.x.page_size;
	default:
		return false;
	}
}

PageMapEntry *page_find_entry(PageMapEntry *pml, int level, uint64_t vaddr, Page_FindEntryCallback callback,
							  void *callback_data) {
	int shift = (12 + (level - 1) * 9);
	uint64_t entry_num = (vaddr >> shift) & (PAGE_MAP_TABLE_LEN - 1);
	PageMapEntry *pme = &pml[entry_num];

	if (!pme->x.present) {
		if (callback) {
			if (!callback(level - 1, pme, callback_data)) return NULL;
		} else {
			return NULL;
		}
	}

	bool is_leaf = pme_is_leaf(*pme, level);

	if (is_leaf) {
		return pme;
	} else {
		return page_find_entry((PageMapEntry *)(pme_addr(*pme)), level - 1, vaddr, callback, callback_data);
	}
}

PageMapEntry *page_current_pml4() { return (PageMapEntry *)asm_read_cr3(); }

static bool map_callback_(int level, PageMapEntry *pme, void *data) {
	uintptr_t page = (uintptr_t)efi_allocate_pages(1);

	memset((void *)page, 0, PAGE_SIZE);
	pme_set_addr(pme, page);
	pme->x.present = 1;
	pme->x.is_read = 1;
	return true;
}

void page_map_addr(uintptr_t paddr, uintptr_t vaddr, int num_page) {
	PageMapEntry *pml4 = page_current_pml4();
	PageMapEntry *new_pml4 = (PageMapEntry *)efi_allocate_pages(1);

	memcpy(new_pml4, pml4, 4096);
	asm_write_cr3((uint64_t)new_pml4);

	for (int i = 0; i < num_page; i++) {
		uintptr_t target_addr = vaddr + PAGE_SIZE * i;
		page_find_entry(new_pml4, 4, target_addr, map_callback_, (void *)target_addr);
	}
}
