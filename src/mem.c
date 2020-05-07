#include "mem.h"

#include "page.h"
#include "physical_memory.h"

static uintptr_t heap_start_;
static uintptr_t heap_sbrk_;
static uintptr_t heap_end_;

void mem_init() {
	ktrace("Initialize kernel heap.");
	const int num = 16;
	uintptr_t block = canonical_addr(256 * (1ULL << 39));

	page_alloc_addr((void *)block, num, true);
	// uintptr_t block = physical_memory_alloc(num);
	heap_start_ = block;
	heap_sbrk_ = block;
	heap_end_ = block + num * PAGE_SIZE;
	ktrace("heap = %018llx~%018llx", heap_start_, heap_end_);
}

uintptr_t mem_sbrk(int diff) {
	uintptr_t old_sbrk = heap_sbrk_;
	heap_sbrk_ += diff;
	ktrace("mem_sbrk %d at %018p", diff, heap_sbrk_);
	return old_sbrk;
}
