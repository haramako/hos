#include "mem.h"

#include "mm.h"
#include "page.h"
#include "physical_memory.h"

static uintptr_t heap_start_;
static uintptr_t heap_sbrk_;
static uintptr_t heap_end_;

MemoryMap *g_kernel_mm;

void mem_init() {
	ktrace("Initialize kernel heap.");
	const int num = 256;
	uintptr_t block = canonical_addr(0xffff800000000000);

	page_alloc_addr((void *)block, num, true, true);
	// uintptr_t block = physical_memory_alloc(num);
	heap_start_ = block;
	heap_sbrk_ = block;
	heap_end_ = block + num * PAGE_SIZE;
	ktrace("heap = %018llx~%018llx", heap_start_, heap_end_);

	g_kernel_mm = mm_new();
	kcheck0(g_kernel_mm);

	PageAttribute attr = {0};
	mm_alloc(g_kernel_mm, (void *)0xffff800000000000, num, &attr);
	mm_print(g_kernel_mm);
}

uintptr_t mem_sbrk(int diff) {
	uintptr_t old_sbrk = heap_sbrk_;
	heap_sbrk_ += diff;
	ktrace("mem_sbrk %d at %018p", diff, heap_sbrk_);
	return old_sbrk;
}
