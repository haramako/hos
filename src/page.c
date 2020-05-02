#include "page.h"

extern inline uint64_t pme_addr(PageMapEntry pml);

const char *pme_flags(PageMapEntry p) {
	static char buf[10];
	buf[0] = '0' + p.x.available;
	buf[1] = p.x.ignored ? 'G' : '-';
	buf[2] = p.x.page_size ? 'S' : '-';
	buf[3] = p.x.accessed ? 'A' : '-';
	buf[4] = p.x.cache_disabled ? 'D' : '-';
	buf[5] = p.x.write_through ? 'W' : '-';
	buf[6] = p.x.is_user ? 'U' : '-';
	buf[7] = p.x.is_read ? 'R' : '-';
	buf[8] = p.x.present ? 'P' : '-';
	buf[9] = '\0';
	return buf;
}

static uint64_t pme_flag(PageMapEntry p) { return p.raw & ((1LLU << 12) - 1); }

const int S = 512;

static void print_pml4_(PageMapEntry *pml4);
static void print_pml3_(PageMapEntry *pml3, uint64_t base_addr);
static void print_pml2_(PageMapEntry *pml2, uint64_t base_addr);
static void print_pml1_(PageMapEntry *pml1, uint64_t base_addr);

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

static void print_pml4_(PageMapEntry *pml4) {
	klog("LV:addr              : value              flags");
	for (int i = 0; i < S; i++) {
		PageMapEntry p4 = pml4[i];
		uint64_t addr = ((uint64_t)i) << 39;
		if (p4.x.present) {
			// klog("L4:%018p: %018p %s", addr, pme_addr(p4), pme_flags(p4));
			PageMapEntry *pml3 = (PageMapEntry *)pme_addr(p4);
			print_pml3_(pml3, addr);
		}
	}
}

static void print_pml3_(PageMapEntry *pml3, uint64_t base_addr) {
	for (int i = 0; i < S; i++) {
		PageMapEntry p3 = pml3[i];
		uint64_t addr = base_addr | ((uint64_t)i) << 30;
		if (p3.x.present) {
			// klog("L3:%018p: %018p %s", base_addr, pme_addr(p3), pme_flags(p3));
			PageMapEntry *pml2 = (PageMapEntry *)pme_addr(p3);
			print_pml2_(pml2, addr);
		}
	}
}

static void print_merged_pme2_(uint64_t base_addr, int merge_start, int merge_end, uint64_t merge_flags) {
	uint64_t start_addr = base_addr + (((uint64_t)merge_start) << 21);
	uint64_t end_addr = base_addr + (((uint64_t)merge_end) << 21);
	int merge_size = merge_end - merge_start;
	PageMapEntry f = {.raw = merge_flags};
	klog("L2:%018p: ------------------ %s %3d %018p~%018p %10s", 0, pme_flags(f), merge_size, start_addr, end_addr - 1,
		 humanize_size(end_addr - start_addr));
}

static void print_pml2_(PageMapEntry *pml2, uint64_t base_addr) {
	int merge_start = -1;
	uint64_t merge_flags = 0;
	for (int i = 0; i < S; i++) {
		PageMapEntry p2 = pml2[i];
		uint64_t addr = base_addr | ((uint64_t)i) << 21;

		// Display merging.
		uint64_t flags = pme_flag(p2);
		bool has_page = (p2.x.present && p2.x.page_size);

		if (merge_start != -1 && (!has_page || (merge_flags != flags))) {
			// Finish display merging.
			print_merged_pme2_(base_addr, merge_start, i, merge_flags);
			merge_start = -1;
		}

		if (merge_start == -1 && has_page) {
			// Start display merging.
			merge_start = i;
			merge_flags = flags;
		}

		if (p2.x.present && !p2.x.page_size) {
			// 4KB page
			// klog("L2:%018p: %018p %s", addr, pme_addr(p2), pme_flags(p2));
			PageMapEntry *pml1 = (PageMapEntry *)pme_addr(p2);
			print_pml1_(pml1, addr);
		}
	}

	if (merge_start != -1) {
		// Finish display merging.
		print_merged_pme2_(base_addr, merge_start, S, merge_flags);
	}
}

static void print_merged_pme1_(uint64_t base_addr, int merge_start, int merge_end, uint64_t merge_flags) {
	uint64_t start_addr = base_addr + (((uint64_t)merge_start) << 12);
	uint64_t end_addr = base_addr + (((uint64_t)merge_end) << 12);
	int merge_size = merge_end - merge_start;
	PageMapEntry f = {.raw = merge_flags};
	klog("L1:%018p: ------------------ %s %3d %018p~%018p %10s", 0, pme_flags(f), merge_size, start_addr, end_addr - 1,
		 humanize_size(end_addr - start_addr));
}
static void print_pml1_(PageMapEntry *pml1, uint64_t base_addr) {
	int merge_start = -1;
	uint64_t merge_flags = 0;
	bool has_page = false;
	for (int i = 0; i < S; i++) {
		PageMapEntry p1 = pml1[i];
		uint64_t addr = base_addr | ((uint64_t)i) << 12;

		// Display merging.
		uint64_t flags = pme_flag(p1);
		bool has_page = p1.x.present;

		if (merge_start != -1 && (!has_page || (merge_flags != flags))) {
			// Finish display merging.
			print_merged_pme1_(base_addr, merge_start, i, merge_flags);
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
		print_merged_pme1_(base_addr, merge_start, S, merge_flags);
	}
}

void page_map_entry_print(PageMapEntry *pml4) {

	*((int *)0x800000) = 1;

	print_pml4_(pml4);
#if 0
	for (int i4 = 0; i4 < S; i4++) {
		PageMapEntry p3 = pml4[i4];
		if (p3.x.present) {
			klog("L4:%02d:  :  :  : %018p %s", i4, pme_addr(p3), pme_flags(p3));
			PageMapEntry *pml3 = (PageMapEntry *)pme_addr(p3);
			for (int i3 = 0; i3 < S; i3++) {
				PageMapEntry p2 = pml3[i3];
				if (p2.x.present) {
					klog("L3:%02d:%02d:  :  : %018p %s", i4, i3, pme_addr(p2), pme_flags(p2));
					PageMapEntry *pml2 = (PageMapEntry *)pme_addr(p2);
					for (int i2 = 0; i2 < S; i2++) {
						PageMapEntry p1 = pml2[i2];
						if (p1.x.present) {
							if (p1.x.page_size) {
								// 4MB page
								uint64_t addr = ((uint64_t)i4 << 38) | ((uint64_t)i3 << 29) | ((uint64_t)i2 << 20);
								klog("L2:%02d:%02d:%02d:  : ------------------ %s %018p", i4, i3, i2, pme_flags(p1),
									 addr);
							} else {
								// 4KB page
								klog("L2:%02d:%02d:%02d:  : %016p %s", i4, i3, i2, pme_addr(p1), pme_flags(p1));
								PageMapEntry *pml1 = (PageMapEntry *)pme_addr(p1);
								for (int i1 = 0; i1 < S; i1++) {
									PageMapEntry p0 = pml1[i1];
									if (p0.x.present) {
										uint64_t addr = ((uint64_t)i4 << 38) | ((uint64_t)i3 << 29) |
														((uint64_t)i2 << 20) | ((uint64_t)i1 << 12);
										klog("L1:%02d:%02d:%02d:%02d: %016p %s %018p", i4, i3, i2, i1, pme_addr(p1),
											 pme_flags(p3), addr);
									}
								}
							}
						}
					}
				}
			}
		}
	}
#endif
}
