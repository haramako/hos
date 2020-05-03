#include "mem.h"

#include "page.h"
#include "physical_memory.h"

static uintptr_t heap_start_;
static uintptr_t heap_sbrk_;
static uintptr_t heap_end_;

void mem_init() {
	ktrace("Initialize kernel heap.");
	const int num = 8;
	uintptr_t block = canonical_addr(256 * (1ULL << 39));
	klog("block %018p", block);

	page_alloc_addr_prelude((void *)block, num);
	// uintptr_t block = physical_memory_alloc(num);
	heap_start_ = block;
	heap_sbrk_ = block;
	heap_end_ = block + num * PAGE_SIZE;
	ktrace("heap = %016llx~%016llx", heap_start_, heap_end_);
}

uintptr_t mem_sbrk(int diff) {
	heap_sbrk_ += diff;
	ktrace("mem_sbrk %d at %016p", diff, heap_sbrk_);
	return heap_sbrk_;
}
