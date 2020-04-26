#include "efi.h"

int efi_memory_map_get_count(EFI_MemoryMap *mm)
{
	return (int)(mm->bytes_used / mm->descriptor_size);
}

EFI_MemoryDescriptor* efi_memory_map_get_descriptor(EFI_MemoryMap *mm, int index)
{
	return (EFI_MemoryDescriptor*)(&mm->buf[index * mm->descriptor_size]);
}

void efi_memory_descriptor_print(EFI_MemoryDescriptor *md)
{
}
