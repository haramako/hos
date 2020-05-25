#pragma once

#include "common.h"

#include "boot_param.h"
#include "efi/memory_map.h"

int efi_memory_map_get_count(EFI_MemoryMap *mm);
EFI_MemoryDescriptor *efi_memory_map_get_descriptor(EFI_MemoryMap *mm, int idx);
void efi_memory_map_print(EFI_MemoryMap *mm);

void efi_memory_descriptor_print(EFI_MemoryDescriptor *md, int index);
