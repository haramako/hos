#include "physical_memory.h"

#include "asm.h"
#include "boot_param.h"
#include "memory_map_util.h"
#include "page.h"

static PhysicalMemory pm_;

static void physical_memory_add_page(uintptr_t physical_start, size_t pages);

void physical_memory_init(EFI_MemoryMap *memory_map) {
	ktrace("Initialize physical memory.");
	pm_.block_len = 0;

	int len = efi_memory_map_get_count(memory_map);
	for (int i = 0; i < len; i++) {
		EFI_MemoryDescriptor *desc = efi_memory_map_get_descriptor(memory_map, i);
		ktrace("pg %d %p %p %d", desc->number_of_pages, desc->physical_start, desc->attribute, desc->type);
		if (desc->type != kConventionalMemory) continue;
		physical_memory_add_page(desc->physical_start, desc->number_of_pages);
	}
}

static void physical_memory_add_page(uintptr_t physical_start, size_t pages) {
	pm_.blocks[pm_.block_len].start = physical_start;
	pm_.blocks[pm_.block_len].pages = pages;
	pm_.block_len++;
}

uintptr_t physical_memory_alloc(uintptr_t pages) {
	int len = pm_.block_len;
	for (int i = 0; i < len; i++) {
		if (pm_.blocks[i].pages >= pages) {
			uintptr_t result = pm_.blocks[i].start;
			pm_.blocks[i].start += pages * PAGE_SIZE;
			pm_.blocks[i].pages -= pages;
			ktrace("Allocate physical memory %lld pages at %p.", pages, result);
			return result;
		}
	}
	klog("Cant allocate %d pages.", pages);
	return 0;
}

uintptr_t physmem_v2p(void *addr) { return (uintptr_t)addr; }
