#include "boot_param.h"

#include "acpi.h"
#include "ahci.h"
#include "apic.h"
#include "asm.h"
#include "fs.h"
#include "gdt.h"
#include "hpet.h"
#include "interrupt.h"
#include "mem.h"
#include "memory_map_util.h"
#include "page.h"
#include "pagefault.h"
#include "pci.h"
#include "physical_memory.h"
#include "scheduler.h"
#include "serial.h"
#include "sheet.h"
#include "sheet_painter.h"
#include "smp.h"
#include "syscall.h"
#include "timer.h"

BootParam *g_boot_param;

#include "main_test.inc" // Include test source.

static void pci_test() {
	pci_init();
	pci_print();
	ahci_init();
}

static void init_gdt_and_interrupt_() {
	const int stack_pages = 1024 * 16;
	uintptr_t stack = physical_memory_alloc(stack_pages);
	uintptr_t ist = physical_memory_alloc(stack_pages);
	gdt_init(stack + stack_pages * PAGE_SIZE, ist + stack_pages * PAGE_SIZE);
	interrupt_init();
	pagefault_init();
}

static void init_process_and_scheduler_() {
	scheduler_init();

	interrupt_set_int_handler(0x28, process_timer_handler);
	hpet_set_timer_ns(1, 100 * MSEC, HPET_TC_ENABLE | HPET_TC_USE_PERIODIC_MODE);
}

void kernel_entry(BootParam *boot_param_passed) {
	g_boot_param = boot_param_passed;

	Disable8259PIC();

	serial_init();
	console_init(serial_get_port(1), &g_boot_param->graphics);
	console_set_log_level(CONSOLE_LOG_LEVEL_INFO);

	// Now you can use console_*().

	kinfo("=======================");
	kinfo("Kernel Start");

	syscall_init();
	physical_memory_init(g_boot_param->efi_memory_map);
	page_init();
	mem_init();

	// Now you can use malloc/free.

	apic_init();
	init_gdt_and_interrupt_();

	// Now ready to interrupt (but interrupt flag is not set).

	hpet_init();
	timer_init();

	init_process_and_scheduler_();

	// Now ready to HPET timer.

	// =========================

	kinfo("Kernel Ready!");
	// kinfo("Time %lld", hpet_read_main_counter_value());

	fs_init();

	// console_set_log_level(CONSOLE_LOG_LEVEL_TRACE);

	// test_virtual_memory_map_();
	// paging_test_();

	// fat_test_();
	process_test_(3);
	// test_malloc_();
	// pci_test();
	// mm_test_();
	// smp_test_();
	// sheet_test_();
	// kbreakpoint();
	// shutdown_test_();

	// fs_test_();

	kinfo("Finish.");
	StoreIntFlag(); // Start interrupt.
	for (;;)
		;
}
