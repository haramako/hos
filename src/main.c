#include "liumos.h"

#include "asm.h"
#include "gdt.h"
#include "hpet.h"
#include "interrupt.h"
#include "mem.h"
#include "page.h"
#include "physical_memory.h"
#include "scheduler.h"
#include "serial.h"
#include "timer.h"

#include "main_test.c" // Include test source.

LiumOS *g_liumos;

static void init_process_and_scheduler_() {
	scheduler_init();

	interrupt_set_int_handler(0x28, process_timer_handler);
	hpet_set_timer_ns(1, 100 * MSEC, HPET_TC_ENABLE | HPET_TC_USE_PERIODIC_MODE);
}

static void paging_test_() {
	uint64_t cr0;
	uint64_t cr3 = ReadCR3();
	__asm__(".intel_syntax noprefix\n"
			"mov rax, cr0"
			: "=rax"(cr0));
	klog("CR0 %p", cr0);
	klog("CR3 %p", cr3);

	// int n = *((int*)0x01);
	//*((int*)0x01) = 1;

	PageMapEntry *pml4 = (PageMapEntry *)cr3;
	page_map_entry_print(pml4);
}

void kernel_entry(LiumOS *liumos_passed) {
	g_liumos = liumos_passed;

	Disable8259PIC();

	serial_init();
	console_init(serial_get_port(1));
	console_set_log_level(CONSOLE_LOG_LEVEL_INFO);

	// Now you can use console_*().

	console_write("\n");
	kinfo("=======================");
	kinfo("Kernel Start");

	physical_memory_init(g_liumos->efi_memory_map);
	mem_init();

	// Now you can use malloc/free.

	apic_init();

	const int stack_pages = 1024 * 16;
	uintptr_t stack = physical_memory_alloc(stack_pages);
	uintptr_t ist = physical_memory_alloc(stack_pages);
	gdt_init(stack + stack_pages * PAGE_SIZE, ist + stack_pages * PAGE_SIZE);
	interrupt_init();

	// Now ready to interrupt (but interrupt flag is not set).

	hpet_init((HPET_RegisterSpace *)g_liumos->acpi.hpet->base_address.address);
	timer_init();

	init_process_and_scheduler_();

	// Now ready to HPET timer.

	StoreIntFlag(); // Start interrupt.

	// =========================

	kinfo("Kernel Ready!");
	kinfo("Time %lld", hpet_read_main_counter_value());

	// process_test_();

	paging_test_();

	Die();
}
