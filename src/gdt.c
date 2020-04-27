#include "gdt.h"

#include <string.h>

#include "asm.h"

GDTR g_gdt_gdtr;
GDTDescriptors g_gdt_descriptors;
IA_TSS64 g_ia_tss64;

void set_base_addr_(GDT_TSS64Entry *entry, void *base_addr) {
	uint64_t base_addr_int = (uint64_t)base_addr;
	entry->base_low = base_addr_int & 0xffff;
	entry->base_mid_low = (base_addr_int >> 16) & 0xff;
	entry->base_mid_high = (base_addr_int >> 24) & 0xff;
	entry->base_high = (base_addr_int >> 32) & 0xffffffffUL;
}

void gdt_init(uint64_t kernel_stack_pointer, uint64_t ist1_pointer) {
	// Initialze g_gdt_descriptors.
	g_gdt_descriptors.null_segment = 0;
	g_gdt_descriptors.kernel_code_segment =
		kDescBitTypeCode | kDescBitPresent | kCSDescBitLongMode | kCSDescBitReadable;
	g_gdt_descriptors.kernel_data_segment = kDescBitTypeData | kDescBitPresent | kDSDescBitWritable;
	g_gdt_descriptors.user_code_segment_32 = 0;
	g_gdt_descriptors.user_data_segment =
		kDescBitTypeData | kDescBitPresent | kDSDescBitWritable | (3ULL << kDescBitOfsDPL);
	g_gdt_descriptors.user_code_segment_64 =
		kDescBitTypeCode | kDescBitPresent | kCSDescBitLongMode | kCSDescBitReadable | (3ULL << kDescBitOfsDPL);

	GDT_TSS64Entry *tss = &g_gdt_descriptors.task_state_segment;
	bzero(tss, sizeof(GDT_TSS64Entry));
	tss->attr = 0b1000000010001001 | (3 << 5);
	tss->limit_low = sizeof(g_ia_tss64) - 1;
	set_base_addr_(tss, &g_ia_tss64);
	// assert(0);

	// Initials TSS.
	bzero(&g_ia_tss64, sizeof(g_ia_tss64));
	g_ia_tss64.rsp[0] = kernel_stack_pointer;
	g_ia_tss64.ist[1] = ist1_pointer;

	// Set GDT, segment registers, TSS to processor.
	g_gdt_gdtr.base = (uint64_t *)(&g_gdt_descriptors);
	g_gdt_gdtr.limit = sizeof(GDTDescriptors) - 1;

	WriteGDTR(&g_gdt_gdtr);
	WriteCSSelector(kKernelCSSelector);
	WriteSSSelector(kKernelDSSelector);
	WriteDataAndExtraSegmentSelectors(kKernelDSSelector);
	WriteTaskRegister(kTSS64Selector);
}

void gdt_print() {
	klog("GDT Descriptors");
	for (size_t i = 0; i < (g_gdt_gdtr.limit + 1) / sizeof(uint64_t); i++) {
		klog("%d: %016llx", i, g_gdt_gdtr.base[i]);
	}
}
