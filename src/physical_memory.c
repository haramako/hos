#include "physical_memory.h"

#include "console.h"

static PhysicalMemory pm_;

static void physical_memory_add_page(uintptr_t physical_start, size_t pages);

void physical_memory_init(EFI_MemoryMap *memory_map)
{
	pm_.block_len = 0;
	
	int len = efi_memory_map_get_count(memory_map);
	for( int i=0; i<len; i++){
		EFI_MemoryDescriptor *desc = efi_memory_map_get_descriptor(memory_map, i);
		if( desc->type != kConventionalMemory ) continue;
		physical_memory_add_page(desc->physical_start, desc->number_of_pages);
	}
}

static void physical_memory_add_page(uintptr_t physical_start, size_t pages)
{
	pm_.blocks[pm_.block_len].start = physical_start;
	pm_.blocks[pm_.block_len].pages = pages;
	pm_.block_len++;
}

uintptr_t physical_memory_alloc(uintptr_t pages)
{
	int len = pm_.block_len;
	for( int i=0; i<len; i++){
		if( pm_.blocks[i].pages >= pages ){
			uintptr_t result = pm_.blocks[i].start;
			pm_.blocks[i].start += pages * PAGE_SIZE;
			pm_.blocks[i].pages -= pages;
			return result;
		}
	}
	return 0;
}
