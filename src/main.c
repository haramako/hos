#include "liumos.h"

#include "asm.h"
#include "console.h"
#include "gdt.h"
#include "hpet.h"
#include "mem.h"
#include "physical_memory.h"
#include "serial.h"
#include "interrupt.h"

LiumOS *g_liumos;

void KernelEntry(LiumOS* liumos_passed);

static void test_malloc_()
{
	int *a = (int*)malloc(4);
	int *b = (int*)malloc(4);
	free(a);
	int *c = (int*)malloc(4);
	int n = 99;
	

	klog("a = %p\nb= %p\nc= %p", (void*)a, (void*)b, c);

	klog("KernelEntry %016llx", KernelEntry);
	klog("g_liumos     %016llx", g_liumos);
	klog("n   _       %016llx", &n);
}

static void test_virtual_memory_map_()
{
	EFI_RuntimeServices *runtime_services = g_liumos->loader_info.efi->system_table->runtime_services;
	EFI_MemoryMap *mm = g_liumos->efi_memory_map;
	int res = runtime_services->set_virtual_address_map(sizeof(mm->buf), mm->descriptor_size, mm->key, (void*)mm->buf);
	klog("res = %d", res);
	efi_memory_map_print(g_liumos->efi_memory_map);
}

static void test_memory_map_()
{
	efi_memory_map_print(g_liumos->efi_memory_map);
}

static void test_reset_()
{
	EFI_RuntimeServices *runtime_services = g_liumos->loader_info.efi->system_table->runtime_services;
	runtime_services->reset_system(EfiResetShutdown, 0, 0, NULL);
}

void kernel_entry(LiumOS* liumos_passed)
{
	g_liumos = liumos_passed;

	Disable8259PIC();

	serial_init();
	console_init(serial_get_port(1));
	console_set_log_level(CONSOLE_LOG_LEVEL_INFO);
	console_set_log_level(CONSOLE_LOG_LEVEL_TRACE);
	
	// Now you can use console_*().

	console_write("\n");
	kinfo("=======================");
	kinfo("Kernel start");

	physical_memory_init(g_liumos->efi_memory_map);
	mem_init();

	// Now you can use malloc/free.

	apic_init();

	const int stack_pages = 1024;
	uintptr_t stack = physical_memory_alloc(stack_pages);
	uintptr_t ist = physical_memory_alloc(stack_pages);
	gdt_init(stack + stack_pages * PAGE_SIZE, ist + stack_pages * PAGE_SIZE);
	interrupt_init();

	__asm__("int $0x20");
	
	// Now ready to interrupt.

	hpet_init((HPET_RegisterSpace*)g_liumos->acpi.hpet->base_address.address);
	hpet_set_timer_ms( 0, 100, HPET_TC_USE_PERIODIC_MODE | HPET_TC_ENABLE);
	g_liumos->time_slice_count = 1e12 * 100 / hpet_get_femtosecond_per_count();
	
	// Now ready to HPET timer.

	kinfo("time %lld", hpet_read_main_counter_value());
	kinfo("time %lld", hpet_read_main_counter_value());
	kinfo("time %lld", hpet_read_main_counter_value());
	uint64_t t1 = hpet_read_main_counter_value();
	uint64_t t2 = hpet_read_main_counter_value();
	kinfo("time %lld", t1);
	kinfo("time %lld", t2);

	//__asm__("int3");

	kinfo("Kernel ready!");
	Die();
}
