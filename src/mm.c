#include "mm.h"

MemoryMap *mm_new() {
	MemoryMap *mm = kalloc(MemoryMap);
	return mm;
}

MemoryBlock *mm_map(MemoryMap *mm, void *vaddr, size_t page_num, MemoryBlockAttribute attr) {
	kcheck(page_num > 0, "page_num must not 0.");
	kcheck(mm, "mm must not null.");
	kcheck(mm->block_num < MM_BLOCK_LEN, "Too many blocks.");

	MemoryBlock *block = kalloc(MemoryBlock);
	block->vaddr_start = (uintptr_t)vaddr;
	block->vaddr_end = (uintptr_t)vaddr + PAGE_SIZE * page_num;
	kcheck0(block->vaddr_start < block->vaddr_end);
	block->page_num = page_num;
	block->paddr = malloc_zero(page_num * sizeof(uint64_t));

	block->attr = attr;

	mm->blocks[mm->block_num] = block;
	mm->block_num++;

	return block;
}

void mm_free(MemoryMap *mm, MemoryBlock *block) {
	for (int i = 0; i < MM_BLOCK_LEN; i++) {
		if (mm->blocks[i] == block) {
			mm->blocks[i] = NULL;
			free(block);
			return;
		}
	}
	kpanic("Block not found.");
}

MemoryBlock *mm_find_vaddr(MemoryMap *mm, void *vaddr_) {
	uintptr_t vaddr = (uintptr_t)vaddr_;
	for (int i = 0; i < MM_BLOCK_LEN; i++) {
		MemoryBlock *b = mm->blocks[i];
		if (vaddr >= b->vaddr_start && vaddr < b->vaddr_end) {
			return b;
		}
	}
	return NULL;
}

void mm_print(MemoryMap *mm) {
	klog("MemoryMap %018p", mm);
	klog("idx: vaddr                                 size     aloc");
	for (int i = 0; i < MM_BLOCK_LEN; i++) {
		MemoryBlock *b = mm->blocks[i];
		if (b == NULL) continue;
		int alloc_num = 0;
		for (uint64_t j = 0; j < b->page_num; j++) {
			if (b->paddr[j] != 0) alloc_num++;
		}

		size_t size = b->page_num * PAGE_SIZE;
		klog("%3d: %018p~%018p %8s %4lld", i, b->vaddr_start, b->vaddr_end - 1, humanize_size(size), alloc_num);
	}
}
