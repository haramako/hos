#include "efi.h"

#include "liumos.h"
#include "console.h"

int efi_memory_map_get_count(EFI_MemoryMap *mm)
{
	return (int)(mm->bytes_used / mm->descriptor_size);
}

EFI_MemoryDescriptor* efi_memory_map_get_descriptor(EFI_MemoryMap *mm, int index)
{
	return (EFI_MemoryDescriptor*)(&mm->buf[index * mm->descriptor_size]);
}

void efi_memory_map_print(EFI_MemoryMap *mm)
{
	klog("idx : p-addr                                v-addr             pages    ty attr");
	int len = efi_memory_map_get_count(mm);
	for( int i=0; i<len; i++){
		EFI_MemoryDescriptor *descriptor = efi_memory_map_get_descriptor(mm, i);
		efi_memory_descriptor_print(descriptor, i);
	}
}

static const char *EFI_MEMORY_MAP_TYPE_NAMES[] = {
    "Reserved",
    "LoaderCode",
    "LoaderData",
    "BootServicesCode",
    "BootServicesData",
    "RuntimeServicesCode",
    "RuntimeServicesData",
    "ConventionalMemory",
    "UnusableMemory",
    "ACPIReclaimMemory",
    "ACPIMemoryNVS",
    "MemoryMappedIO",
    "MemoryMappedIOPortSpace",
    "PalCode",
	"PersistentMemory",
	"MaxMemoryType",
};

void efi_memory_descriptor_print(EFI_MemoryDescriptor *md, int index)
{
	klog("%04d: 0x%016llx-0x%016llx 0x%016llx %8lld %2d %s",
		 index,
		 md->physical_start,
		 md->physical_start + md->number_of_pages * 4096 - 1,
		 md->virtual_start,
		 md->number_of_pages,
		 md->attribute,
		 EFI_MEMORY_MAP_TYPE_NAMES[md->type]
		 );
}
