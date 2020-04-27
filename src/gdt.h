#pragma once

#include "common.h"

#include "asm.h"

#define kDescBitTypeData (0b10ULL << 43)
#define kDescBitTypeCode (0b11ULL << 43)

#define kDescBitForSystemSoftware (1ULL << 52)
#define kDescBitPresent (1ULL << 47)
#define kDescBitOfsDPL (45)
#define kDescBitMaskDPL (0b11ULL << kDescBitOfsDPL)
#define kDescBitAccessed (1ULL << 40)

#define kCSDescBitLongMode (1ULL << 53)
#define kCSDescBitReadable (1ULL << 41)

#define kDSDescBitWritable (1ULL << 41)

#define kKernelCSIndex (1)
#define kKernelDSIndex (2)
#define kUserCS32Index (3)
#define kUserDSIndex (4)
#define kUserCS64Index (5)
#define kTSS64Index (6)

#define kKernelCSSelector (kKernelCSIndex << 3)
#define kKernelDSSelector (kKernelDSIndex << 3)
#define kUserCS32Selector (kUserCS32Index << 3 | 3)
#define kUserDSSelector (kUserDSIndex << 3 | 3)
#define kUserCS64Selector (kUserCS64Index << 3 | 3)
#define kTSS64Selector (kTSS64Index << 3)

typedef struct PACKED GDT_TSS64Entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid_low;
	uint16_t attr;
	uint8_t base_mid_high;
	uint32_t base_high;
	uint32_t reserved;
} GDT_TSS64Entry;

static_assert(sizeof(GDT_TSS64Entry) == 16, "Invalid GDT_TSS64Entry size");

typedef struct PACKED GDTDescriptors {
	uint64_t null_segment;
	uint64_t kernel_code_segment;
	uint64_t kernel_data_segment;
	uint64_t user_code_segment_32;
	uint64_t user_data_segment;
	uint64_t user_code_segment_64;
	GDT_TSS64Entry task_state_segment;
} GDTDescriptors;

extern GDTR g_gdt_gdtr;
extern GDTDescriptors g_gdt_descriptors;
extern IA_TSS64 g_ia_tss64;

void gdt_init(uint64_t kernel_stack_pointer, uint64_t ist1_pointer);
void gdt_print();
