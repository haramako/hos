#include "mem.h"

#include "mm.h"
#include "page.h"
#include "physical_memory.h"

static uintptr_t heap_start_;
static uintptr_t heap_sbrk_;
static uintptr_t heap_end_;

MemoryMap *g_kernel_mm;

static bool alloc_page_callback2_(int level, PageMapEntry *pme, void *data) {
	uintptr_t page = physical_memory_alloc(1);
	memset((void *)page, 0, PAGE_SIZE);
	pme_set_addr(pme, page);
	pme->x.present = 1;
	pme->x.is_read = 1;
	// klog("alloc_page lv=%d, pme=%p(%p), addr=%p", level, pme, pme->raw, data);
	return true;
}

static void page_alloc_addr(void *addr, int num_page) {
	PageMapEntry *pml4 = page_current_pml4();
	for (int i = 0; i < num_page; i++) {
		uintptr_t target_addr = (uintptr_t)addr + PAGE_SIZE * i;
		page_find_entry(pml4, 4, target_addr, alloc_page_callback2_, (void *)target_addr);
	}
}

void mem_init() {
	ktrace("Initialize kernel heap.");
	const int num = 512;
	uintptr_t block = canonical_addr(0xffff800000000000);
	page_alloc_addr((void *)block, num);

	heap_start_ = block;
	heap_sbrk_ = block;
	heap_end_ = block + num * PAGE_SIZE;
	ktrace("heap = %018llx~%018llx", heap_start_, heap_end_);

	g_kernel_mm = mm_new();
	kcheck0(g_kernel_mm);

	mm_map(g_kernel_mm, (void *)0xffff800000000000, num, MM_ATTR_USER);
	// mm_print(g_kernel_mm);
}

uintptr_t mem_sbrk(int diff) {
	uintptr_t old_sbrk = heap_sbrk_;
	heap_sbrk_ += diff;
	ktrace("mem_sbrk %d at %018p", diff, heap_sbrk_);
	return old_sbrk;
}

static bool alloc_page_callback_(int level, PageMapEntry *pme, void *data) {
	uintptr_t page = physical_memory_alloc(1);
	memset((void *)page, 0, PAGE_SIZE);
	pme_set_addr(pme, page);
	pme->x.present = 1;
	pme->x.is_read = 1;
	pme->x.is_user = 1;
	// klog("alloc_page lv=%d, pme=%p(%p), paddr=%p, vaddr=%p", level, pme, pme->raw, page, data);
	return true;
}

error_t mem_alloc_addr(PageMapEntry *pml4, MemoryMap *mm, void *vaddr) {
	MemoryBlock *mb = mm_find_vaddr(mm, vaddr);
	// klog("block %p", mb);
	if (mb) {
		page_find_entry(pml4, 4, (uint64_t)vaddr, alloc_page_callback_, mb);
		return ERR_OK;
	} else {
		return ERR_MM_NOT_FOUND;
	}
}

error_t mem_alloc_memory_block(PageMapEntry *pml4, MemoryBlock *mm) {
	for (uintptr_t vaddr = mm->vaddr_start; vaddr < mm->vaddr_end; vaddr += PAGE_SIZE) {
		// klog("alloc block %p", vaddr);
		page_find_entry(pml4, 4, (uint64_t)vaddr, alloc_page_callback_, NULL);
	}
	return ERR_OK;
}
