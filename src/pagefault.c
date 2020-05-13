#include "pagefault.h"

#include "interrupt.h"
#include "mem.h"
#include "mm.h"
#include "page.h"
#include "physical_memory.h"
#include "scheduler.h"

static bool alloc_page_callback_(int level, PageMapEntry *pme, void *data) {
	uintptr_t page = physical_memory_alloc(1);
	pme_set_addr(pme, page);
	pme->x.present = 1;
	pme->x.is_read = 1;
	pme->x.is_user = 1;
	return true;
}

static void page_fault_handler_(uint64_t intcode, InterruptInfo *info) {
	uint64_t addr = ReadCR2();
	ktrace("Page Fault at %018p, target addr = %018p, error = %08x", info->int_ctx.rip, addr, info->error_code);

	Process *process = scheduler_current_process();
	MemoryBlock *mb = mm_find_vaddr(process->mm, (void *)addr);
	klog("block %p", mb);
	if (mb) {
		page_find_entry((PageMapEntry *)process->ctx->cpu_context.cr3, 4, addr, alloc_page_callback_, mb);
	} else {
		kpanic("Page Fault.");
	}
}

void pagefault_init() { interrupt_set_int_handler(0x0e, page_fault_handler_); }
