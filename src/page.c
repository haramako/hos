#include "page.h"

extern inline uint64_t pme_addr(PageMapEntry pml);

const char *pme_flags(PageMapEntry pml) {
	static char buf[10];
	buf[0] = '0' + pml.x.available;
	buf[1] = pml.x.ignored ? 'G' : '-';
	buf[2] = pml.x.page_size ? 'S' : '-';
	buf[3] = pml.x.accessed ? 'A' : '-';
	buf[4] = pml.x.cache_disabled ? 'D' : '-';
	buf[5] = pml.x.write_through ? 'W' : '-';
	buf[6] = pml.x.is_user ? 'U' : '-';
	buf[7] = pml.x.is_read ? 'R' : '-';
	buf[8] = pml.x.present ? 'P' : '-';
	buf[9] = '\0';
	return buf;
}

void page_map_entry_print(PageMapEntry *pml4) {
	const int S = 512;

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
}
