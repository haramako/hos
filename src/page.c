#include "page.h"

#include "asm.h"
#include "physical_memory.h"

extern inline uint64_t canonical_addr(uint64_t addr);
extern inline uint64_t pme_addr(PageMapEntry pme);
extern inline void pme_set_addr(PageMapEntry *pme, uint64_t paddr);

#define PME_MARK_NONE 0ULL
#define PME_MARK_FIXED (1ULL << 2) /// Fixed for EFI and bootstrap.

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

static bool pme_is_fixed_(PageMapEntry pme) { return pme.x.available & PME_MARK_FIXED; }

static uint64_t pme_flag(PageMapEntry p) { return p.raw & ((1LLU << 12) - 1); }

static void print_pml4_(PMEDisplayConfig *conf, PageMapEntry *pml4);
static void print_pml3_(PMEDisplayConfig *conf, PageMapEntry *pml3, uint64_t base_addr);
static void print_pml2_(PMEDisplayConfig *conf, PageMapEntry *pml2, uint64_t base_addr);
static void print_pml1_(PMEDisplayConfig *conf, PageMapEntry *pml1, uint64_t base_addr);

static const char *humanize_size(uint64_t size) {
	static char buf[16];
	if (size >= (1LLU << 30)) {
		snprintf(buf, sizeof(buf), "%4ld GB", size >> 30);
	} else if (size >= (1LLU << 20)) {
		snprintf(buf, sizeof(buf), "%4ld MB", size >> 20);
	} else if (size >= (1LLU << 10)) {
		snprintf(buf, sizeof(buf), "%4ld KB", size >> 10);
	} else {
		snprintf(buf, sizeof(buf), "%ld", size);
	}
	return buf;
}

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
		if (p3.x.present) {
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
		bool has_page = (p2.x.present && p2.x.page_size);

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

		if (p2.x.present && p2.x.page_size) {
			// klog("L2: %018p~%018p %10s %3d %s %018p", addr, addr + (1UL<<21) - 1, humanize_size(1UL<<21), 1,
			// pme_flags(p2), pme_addr(p2) );
		}

		if (p2.x.present && !p2.x.page_size) {
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
		bool has_page = p1.x.present;

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

void page_map_entry_print(PageMapEntry *pml4) {
	PMEDisplayConfig conf = {.mask = ~0x60, .show_nonleaf = true, .show_fixed = false};
	print_pml4_(&conf, pml4);
}

typedef struct FindEntryOpt_ {
	uint64_t vaddr;
	bool map_if_not_found;
} FindEntryOpt;

typedef struct FindEntryResult_ {
	PageMapEntry *entry;
	uintptr_t paddr;
	int level;
	bool found;
} FindEntryResult;

FindEntryResult find_entry_(FindEntryOpt *opt, PageMapEntry *pml, int level) {
	int shift = (12 + (level - 1) * 9);
	uint64_t entry_num = (opt->vaddr >> shift) & (PAGE_MAP_TABLE_LEN - 1);
	PageMapEntry *pme = &pml[entry_num];
	// klog("%d %p %d %p", level, vaddr, entry_num, pme);

	if (!pme->x.present) {
		if (opt->map_if_not_found) {
			uintptr_t new_pml = physical_memory_alloc(1);
			bzero((void *)new_pml, 4096);
			pme_set_addr(pme, new_pml);
			pme->x.present = true;
			pme->x.is_read = true;
			ktrace("Init page map entry at %018p, level %d", pme, level);
		} else {
			FindEntryResult result = {.entry = pme, .level = level - 1, 0};
			return result;
		}
	}

	bool is_leaf = pme_is_leaf_(*pme, level);

	if (is_leaf) {
		uint64_t mask = (1 << shift) - 1;
		uint64_t paddr = pme_addr(*pme) | (opt->vaddr & mask);
		FindEntryResult result = {.found = true, .entry = pme, .level = level - 1, .paddr = paddr};
		return result;
	} else {
		return find_entry_(opt, (PageMapEntry *)(pme_addr(*pme)), level - 1);
	}
}

uintptr_t page_v2p(PageMapEntry *pml4, void *addr) {
	FindEntryOpt opt = {.vaddr = (uint64_t)addr, .map_if_not_found = false};
	FindEntryResult el = find_entry_(&opt, pml4, 4);
	return el.paddr;
}

PageMapEntry *copy_page_map_table_(PageMapEntry *pml, int level) {
	PageMapEntry *new_pml = (PageMapEntry *)physical_memory_alloc(1);
	kcheck(new_pml, "Can't allocate new page map table.");
	bzero(new_pml, 4096);

	int num_copied = 0;
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry pme = pml[i];
		if (!pme.x.present) continue;
		num_copied++;
		if (pme_is_fixed_(pme)) {
			new_pml[i] = pme;
		} else {
			if (pme_is_leaf_(pme, level - 1)) {
				new_pml[i] = pme;
			} else {
				copy_page_map_table_((PageMapEntry *)pme_addr(pme), level - 1);
			}
		}
	}
	ktrace("Copy page map table %018p level %d, %d entries", pml, level, num_copied);
	return new_pml;
}

PageMapEntry *page_copy_page_map_table(PageMapEntry *pml4) { return copy_page_map_table_(pml4, 4); }

void page_init(PageMapEntry *pml4) {
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry *pme = &pml4[i];
		if (!pme->x.present) continue;
		pme->x.available |= PME_MARK_FIXED;
	}
}

PageMapEntry *get_pml4_() { return (PageMapEntry *)ReadCR3(); }

void page_alloc_addr(void *addr, int num_page) {
	kcheck((((uintptr_t)addr) & (PAGE_SIZE - 1)) == 0, "Invalid addr. addr must aligned page size.");
	FindEntryOpt opt = {.vaddr = (uint64_t)addr, .map_if_not_found = false};
	FindEntryResult r = find_entry_(&opt, get_pml4_(), 4);
	klog("%p", r.entry);
}

void page_alloc_addr_prelude(void *addr, int num_page) {

	kcheck((((uintptr_t)addr) & (PAGE_SIZE - 1)) == 0, "Invalid addr. addr must aligned page size.");
	for (int i = 0; i < num_page; i++) {
		FindEntryOpt opt = {.vaddr = (uint64_t)addr, .map_if_not_found = (uint64_t)addr + (uint64_t)i * PAGE_SIZE};
		FindEntryResult r = find_entry_(&opt, get_pml4_(), 4);
		kcheck(r.found, "BUG");
	}

	page_map_entry_print(get_pml4_());
}
