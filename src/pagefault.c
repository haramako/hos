#include "pagefault.h"

#include "interrupt.h"
#include "mem.h"
#include "scheduler.h"

static void page_fault_handler_(uint64_t intcode, InterruptInfo *info) {
	uint64_t vaddr = ReadCR2();
	ktrace("Page Fault at %018p, target addr = %018p, error = %08x", info->int_ctx.rip, vaddr, info->error_code);

	Process *process = scheduler_current_process();
	if (mem_alloc_addr((PageMapEntry *)process->ctx->cpu_context.cr3, process->mm, (void *)vaddr)) {
		kpanic("Page Fault.");
	}
}

void pagefault_init() { interrupt_set_int_handler(0x0e, page_fault_handler_); }
