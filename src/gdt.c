#include "gdt.h"

#include <string.h>

#include "asm.h"
#include "console.h"

GDTR g_gdt_gdtr;
GDTDescriptors g_gdt_descriptors;
IA_TSS64 g_ia_tss64;

void gdt_init(uint64_t kernel_stack_pointer, uint64_t ist1_pointer)
{
  g_gdt_descriptors.null_segment = 0;
  g_gdt_descriptors.kernel_code_segment = kDescBitTypeCode | kDescBitPresent |
                                     kCSDescBitLongMode | kCSDescBitReadable;
  g_gdt_descriptors.kernel_data_segment =
      kDescBitTypeData | kDescBitPresent | kDSDescBitWritable;
  g_gdt_descriptors.user_code_segment_32 = 0;
  g_gdt_descriptors.user_data_segment = kDescBitTypeData | kDescBitPresent |
                                   kDSDescBitWritable |
                                   (3ULL << kDescBitOfsDPL);
  g_gdt_descriptors.user_code_segment_64 = kDescBitTypeCode | kDescBitPresent |
                                      kCSDescBitLongMode | kCSDescBitReadable |
                                      (3ULL << kDescBitOfsDPL);
  bzero(&g_gdt_descriptors.task_state_segment, sizeof(GDT_TSS64Entry));
  g_gdt_descriptors.task_state_segment.attr = 0b1000000010001001 | (3 << 5);
  //g_gdt_descriptors.task_state_segment.SetBaseAddr(&tss64_);
  //g_gdt_descriptors.task_state_segment.SetLimit(sizeof(tss64_) - 1);
  bzero(&g_ia_tss64, sizeof(g_ia_tss64));
  g_ia_tss64.rsp[0] = kernel_stack_pointer;
  g_ia_tss64.ist[1] = ist1_pointer;

  g_gdt_gdtr.base = (uint64_t*)(&g_gdt_descriptors);
  g_gdt_gdtr.limit = sizeof(GDTDescriptors) - 1;

  WriteGDTR(&g_gdt_gdtr);
  WriteCSSelector(kKernelCSSelector);
  WriteSSSelector(kKernelDSSelector);
  WriteDataAndExtraSegmentSelectors(kKernelDSSelector);
  WriteTaskRegister(kTSS64Selector);
}

void gdt_print()
{
  for (size_t i = 0; i < (g_gdt_gdtr.limit + 1) / sizeof(uint64_t); i++) {
    klog("ent %016llx", g_gdt_gdtr.base[i]);
  }
}
