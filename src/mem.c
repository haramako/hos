#include "mem.h"

#include "physical_memory.h"

uintptr_t heap_start_;
uintptr_t heap_sbrk_;
uintptr_t heap_end_;

void mem_init() {
	ktrace("Initialize kernel heap.");
	const int num = 2048;
	uintptr_t block = physical_memory_alloc(num);
	heap_start_ = block;
	heap_sbrk_ = block;
	heap_end_ = block + num * PAGE_SIZE;
	ktrace("heap = %016llx~%016llx", heap_start_, heap_end_);
}

uintptr_t mem_sbrk(int diff) {
	heap_sbrk_ += diff;
	return heap_sbrk_;
}
