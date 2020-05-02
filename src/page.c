#include "page.h"

extern inline uint64_t canonical_addr(uint64_t addr);
extern inline uint64_t pme_addr(PageMapEntry pml);

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
	klog("LV: v-addr                                      size num flags    table-address");
	for (int i = 0; i < PAGE_MAP_TABLE_LEN; i++) {
		PageMapEntry p4 = pml4[i];
		uint64_t addr = canonical_addr(((uint64_t)i) << 39);
		if (p4.x.present) {
			if (conf->show_nonleaf)
				klog("L4   : %018p~                                  %s %018p", addr, pme_flags(p4), pme_addr(p4));
			PageMapEntry *pml3 = (PageMapEntry *)pme_addr(p4);
			print_pml3_(conf, pml3, addr);
		}
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
	PMEDisplayConfig conf = {.mask = ~0x60, .show_nonleaf = true};
	print_pml4_(&conf, pml4);
}
