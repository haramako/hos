#include "page.h"

#include "asm.h"
#include "interrupt.h"
#include "mem.h"
#include "mm.h"
#include "physical_memory.h"

extern inline uint64_t canonical_addr(uint64_t addr);
extern inline uint64_t pme_addr(PageMapEntry pme);
extern inline void pme_set_addr(PageMapEntry *pme, uint64_t paddr);
extern inline void *page_align(void *addr);

#define PME_MARK_NONE 0ULL
#define PME_MARK_MAPPED (1ULL << 1) /// Fixed for EFI and bootstrap.
#define PME_MARK_FIXED (1ULL << 2)  /// Fixed for EFI and bootstrap.

static PageMapEntry *get_pml4_() { return (PageMapEntry *)ReadCR3(); }

const char *pme_flags(PageMapEntry p) {
	static char buf[10];
	buf[0] = '0' + p.x.available;
	buf[1] = p.x.page_size ? 'S' : '-';
	buf[2] = p.x.accessed ? 'A' : '-';
	buf[3] = p.x.cache_disabled ? 'D' : '-';
	buf[4] = p.x.write_through ? 'W' : '-';
	buf[5] = p.x.is_user ? 'U' : '-';
	buf[6] = p.x.is_read ? 'R' : '-';
	buf[7] = p.x.present ? 'P' : '-';
	buf[8] = '\0';
	return buf;
}

static bool pme_is_leaf_(PageMapEntry pme, int level) {
	bool is_leaf;
	switch (level) {
	case 1:
		return true;
	case 2:
		return pme.x.page_size;
	default:
		return false;
	}
}

static inline bool pme_is_fixed_(PageMapEntry pme) { return pme.x.available & PME_MARK_FIXED; }
static inline uint64_t pme_mapped(PageMapEntry p) { return p.x.present; }
static inline uint64_t pme_flag(PageMapEntry p) { return p.raw & ((1LLU << 12) - 1); }

static void print_pml4_(PMEDisplayConfig *conf, PageMapEntry *pml4);
static void print_pml3_(PMEDisplayConfig *conf, PageMapEntry *pml3, uint64_t base_addr);
static void print_pml2_(PMEDisplayConfig *conf, PageMapEntry *pml2, uint64_t base_addr);
static void print_pml1_(PMEDisplayConfig *conf, PageMapEntry *pml1, uint64_t base_addr);

static void print_pml4_(PMEDisplayConfig *conf, PageMapEntry *pml4) {
	klog("LV   : v-addr                                      size num flags    table-address");
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry p4 = pml4[i];
		uint64_t addr = canonical_addr(((uint64_t)i) << 39);
		if (!p4.x.present) continue;
		if (!conf->show_fixed && pme_is_fixed_(p4)) continue;
		if (conf->show_nonleaf)
			klog("L4   : %018p~                                  %s %018p", addr, pme_flags(p4), pme_addr(p4));
		PageMapEntry *pml3 = (PageMapEntry *)pme_addr(p4);
		print_pml3_(conf, pml3, addr);
	}
}

static void print_pml3_(PMEDisplayConfig *conf, PageMapEntry *pml3, uint64_t base_addr) {
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry p3 = pml3[i];
		uint64_t addr = canonical_addr(base_addr | ((uint64_t)i) << 30);
		if (pme_mapped(p3)) {
			if (conf->show_nonleaf)
				klog(" L3  : %018p~                                  %s %018p", addr, pme_flags(p3), pme_addr(p3));
			PageMapEntry *pml2 = (PageMapEntry *)pme_addr(p3);
			print_pml2_(conf, pml2, addr);
		}
	}
}

static void print_merged_pme_(int pme_level, uint64_t base_addr, int merge_start, int merge_end, uint64_t merge_flags) {
	uint64_t start_addr = base_addr + (((uint64_t)merge_start) << (3 + pme_level * 9));
	uint64_t end_addr = base_addr + (((uint64_t)merge_end) << (3 + pme_level * 9));
	int merge_size = merge_end - merge_start;
	PageMapEntry f = {.raw = merge_flags};
	const char *indent = (pme_level == 1) ? "   L1" : "  L2 ";
	klog("%s: %018p~%018p %10s %3d %s", indent, start_addr, end_addr - 1, humanize_size(end_addr - start_addr),
		 merge_size, pme_flags(f));
}

static void print_pml2_(PMEDisplayConfig *conf, PageMapEntry *pml2, uint64_t base_addr) {
	int merge_start = -1;
	uint64_t merge_flags = 0;
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry p2 = pml2[i];
		uint64_t addr = canonical_addr(base_addr | ((uint64_t)i) << 21);

		// Display merging.
		uint64_t flags = pme_flag(p2) & conf->mask;
		bool has_page = (pme_mapped(p2) && p2.x.page_size);

		if (merge_start != -1 && (!has_page || (merge_flags != flags))) {
			// Finish display merging.
			print_merged_pme_(2, base_addr, merge_start, i, merge_flags);
			merge_start = -1;
		}

		if (merge_start == -1 && has_page) {
			// Start display merging.
			merge_start = i;
			merge_flags = flags;
		}

		if (pme_mapped(p2) && p2.x.page_size) {
			// klog("L2: %018p~%018p %10s %3d %s %018p", addr, addr + (1UL<<21) - 1, humanize_size(1UL<<21), 1,
			// pme_flags(p2), pme_addr(p2) );
		}

		if (pme_mapped(p2) && !p2.x.page_size) {
			// 4KB page
			if (conf->show_nonleaf)
				klog("  L2 : %018p~                                  %s %018p", addr, pme_flags(p2), pme_addr(p2));
			PageMapEntry *pml1 = (PageMapEntry *)pme_addr(p2);
			print_pml1_(conf, pml1, addr);
		}
	}

	if (merge_start != -1) {
		// Finish display merging.
		print_merged_pme_(2, base_addr, merge_start, PAGE_MAP_TABLE_LEN, merge_flags);
	}
}

static void print_pml1_(PMEDisplayConfig *conf, PageMapEntry *pml1, uint64_t base_addr) {
	int merge_start = -1;
	uint64_t merge_flags = 0;
	bool has_page = false;
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry p1 = pml1[i];
		uint64_t addr = canonical_addr(base_addr | ((uint64_t)i) << 12);

		// Display merging.
		uint64_t flags = pme_flag(p1) & conf->mask;
		bool has_page = pme_mapped(p1);

		if (merge_start != -1 && (!has_page || (merge_flags != flags))) {
			// Finish display merging.
			print_merged_pme_(1, base_addr, merge_start, i, merge_flags);
			merge_start = -1;
		}

		if (merge_start == -1 && has_page) {
			// Start display merging.
			merge_start = i;
			merge_flags = flags;
		}
	}

	if (merge_start != -1) {
		// Finish display merging.
		print_merged_pme_(1, base_addr, merge_start, PAGE_MAP_TABLE_LEN, merge_flags);
	}
}

void pme_print(PageMapEntry *pml4) {
	PMEDisplayConfig conf = {.mask = ~0x60, .show_nonleaf = true, .show_fixed = false};
	print_pml4_(&conf, pml4);
}

typedef struct FindEntryOpt_ {
	uint64_t vaddr;
	bool map_if_not_found;
	bool alloc;
	bool is_user;
} FindEntryOpt;

typedef struct FindEntryResult_ {
	PageMapEntry *entry;
	uintptr_t paddr;
	int level;
	bool found;
} FindEntryResult;

PageMapEntry *find_entry_(PageMapEntry *pml, int level, uint64_t vaddr, Page_FindEntryCallback callback,
						  void *callback_data) {
	int shift = (12 + (level - 1) * 9);
	uint64_t entry_num = (vaddr >> shift) & (PAGE_MAP_TABLE_LEN - 1);
	PageMapEntry *pme = &pml[entry_num];
	// klog("%d %p %d %p", level, opt->vaddr, entry_num, pme);

	if (!pme_mapped(*pme)) {
		if (callback) {
			if (!callback(level - 1, pme, callback_data)) return NULL;
		} else {
			return NULL;
		}
	}

#if 0
	if (opt->alloc && !pme->x.present) {
		uintptr_t new_pml = physical_memory_alloc(1);
		bzero((void *)new_pml, PAGE_SIZE);
		pme_set_addr(pme, new_pml);
		pme->x.present = true;
		if (opt->vaddr < 0xffff800000000000) {
			pme->x.is_user = true;
		}
		pme->x.is_read = true;
		ktrace("Init page map entry at %018p, level %d, paddr=%018p", pme, level, new_pml);
	}
#endif

	bool is_leaf = pme_is_leaf_(*pme, level);

	if (is_leaf) {
		return pme;
	} else {
		return find_entry_((PageMapEntry *)(pme_addr(*pme)), level - 1, vaddr, callback, callback_data);
	}
}

uintptr_t page_v2p(PageMapEntry *pml4, void *addr) {
	PageMapEntry *pme = find_entry_(pml4, 4, (uint64_t)addr, NULL, NULL);
	if (pme) {
		return pme_addr(*pme);
	} else {
		return 0;
	}
}

PageMapEntry *copy_page_map_table_(PageMapEntry *pml, int level) {
	PageMapEntry *new_pml = (PageMapEntry *)physical_memory_alloc(1);
	kcheck(new_pml, "Can't allocate new page map table.");
	bzero(new_pml, 4096);

	int num_copied = 0;
	int num_fixed = 0;
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry pme = pml[i];
		if (!pme_mapped(pme)) continue;
		if (pme_is_fixed_(pme)) {
			num_fixed++;
			new_pml[i] = pme;
		} else {
			num_copied++;
			if (pme_is_leaf_(pme, level)) {
				new_pml[i] = pme;
			} else {
				PageMapEntry *child = copy_page_map_table_((PageMapEntry *)pme_addr(pme), level - 1);
				new_pml[i] = pme;
				pme_set_addr(&new_pml[i], (uint64_t)child);
			}
		}
	}
	ktrace("Copy page map table %018p level %d, copy %d entries, fixed %d entries", pml, level, num_copied, num_fixed);
	return new_pml;
}

PageMapEntry *page_copy_page_map_table(PageMapEntry *pml4) { return copy_page_map_table_(pml4, 4); }

void page_init() {
	PageMapEntry *pml4 = get_pml4_();
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry *pme = &pml4[i];
		if (!pme_mapped(*pme)) continue;
		pme->x.available |= PME_MARK_FIXED;
	}
}

static bool alloc_page_callback_(int level, PageMapEntry *pme, void *data) {
	uintptr_t page = physical_memory_alloc(1);
	pme_set_addr(pme, page);
	pme->x.present = 1;
	pme->x.is_read = 1;
	return true;
}

static void handler_page_fault_(uint64_t intcode, InterruptInfo *info) {
	uint64_t addr = ReadCR2();
	ktrace("Page Fault at %018p, target addr = %018p, error = %08x", info->int_ctx.rip, addr, info->error_code);
	FindEntryOpt opt = {.vaddr = (uint64_t)page_align((void *)addr), .map_if_not_found = false, .alloc = true};
	// FindEntryResult r = find_entry_(get_pml4_(), page_4);
	// klog("r=%p, pme=%s", r.paddr, pme_flags(*r.entry));

	MemoryBlock *mb = mm_find_vaddr(g_kernel_mm, (void *)addr);
	klog("block %p", mb);
	if (mb) {
		find_entry_(get_pml4_(), 4, addr, alloc_page_callback_, mb);
	} else {
		kpanic("Page Fault.");
	}
}

void page_init_interrupt() { interrupt_set_int_handler(0x0e, handler_page_fault_); }

static bool alloc_page_callback2_(int level, PageMapEntry *pme, void *data) {
	uintptr_t page = physical_memory_alloc(1);
	pme_set_addr(pme, page);
	pme->x.present = 1;
	pme->x.is_read = 1;
	klog("alloc_page lv=%d, pme=%p(%p), addr=%p", level, pme, pme->raw, data);
	return true;
}

void page_pme_alloc_addr(PageMapEntry *pml4, void *addr, int num_page, bool alloc, bool is_user) {
	kcheck((((uintptr_t)addr) & (PAGE_SIZE - 1)) == 0, "Invalid addr. addr must aligned page size.");

	for (int i = 0; i < num_page; i++) {
		uintptr_t target_addr = (uintptr_t)addr + PAGE_SIZE * i;
		find_entry_(get_pml4_(), 4, target_addr, alloc_page_callback2_, (void *)target_addr);
	}
}

void page_alloc_addr(void *addr, int num_page, bool alloc, bool is_user) {
	page_pme_alloc_addr(get_pml4_(), addr, num_page, alloc, is_user);
}

void page_memcpy(PageMapEntry *dest_pml4, void *dest, void *src, size_t size) {
	for (size_t i = 0; i < size; i++) {
		uintptr_t addr_rest = ((uintptr_t)dest + i) & (PAGE_SIZE - 1);
		PageMapEntry *pme = find_entry_(dest_pml4, 4, (uint64_t)page_align((uint8_t *)dest + i), NULL, NULL);
		kcheck(pme, "Page not mapped");
		uint8_t *a = (uint8_t *)pme_addr(*pme) + addr_rest;
		// klog("%lld, %p", i, a);
		*a = ((uint8_t *)src)[i];
	}
}

PageMapEntry *page_current_pml4() { return get_pml4_(); }
