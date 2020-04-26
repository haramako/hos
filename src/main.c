#include "liumos.h"

#include "asm.h"
#include "console.h"
#include "gdt.h"
#include "mem.h"
#include "physical_memory.h"
#include "serial.h"

LiumOS *liumos_;

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
	klog("liumos_     %016llx", liumos_);
	klog("n   _       %016llx", &n);
}

static void test_virtual_memory_map_()
{
	EFI_RuntimeServices *runtime_services = liumos_->loader_info.efi->system_table->runtime_services;
	EFI_MemoryMap *mm = liumos_->efi_memory_map;
	int res = runtime_services->set_virtual_address_map(sizeof(mm->buf), mm->descriptor_size, mm->key, (void*)mm->buf);
	klog("res = %d", res);
	efi_memory_map_print(liumos_->efi_memory_map);
}

static void test_memory_map_()
{
	efi_memory_map_print(liumos_->efi_memory_map);
}

static void test_reset_()
{
	EFI_RuntimeServices *runtime_services = liumos_->loader_info.efi->system_table->runtime_services;
	runtime_services->reset_system(EfiResetShutdown, 0, 0, NULL);
}

void KernelEntry(LiumOS* liumos_passed)
{
	liumos_ = liumos_passed;

	serial_init();
	console_init(serial_get_port(1));
	
	// Now you can use console_*().

	klog("\n=======================");
	klog("Kernel start");

	physical_memory_init(liumos_->efi_memory_map);
	mem_init();

	// Now you can use malloc/free.

	const int stack_pages = 1024;
	uintptr_t stack = physical_memory_alloc(stack_pages);
	uintptr_t ist = physical_memory_alloc(stack_pages);
	gdt_init(stack + stack_pages * PAGE_SIZE, ist + stack_pages * PAGE_SIZE);

	gdt_print();

	klog("Kernel OK!");
	Die();
}
