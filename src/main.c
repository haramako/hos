#include "liumos.h"

#include "asm.h"
#include "console.h"
#include "mem.h"
#include "physical_memory.h"
#include "serial.h"

LiumOS *liumos_;

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

	int *a = (int*)malloc(4);
	int *b = (int*)malloc(4);
	free(a);
	int *c = (int*)malloc(4);
	int n = 99;
	

	klog("a = %p\nb= %p\nc= %p", (void*)a, (void*)b, c);

	klog("KernelEntry %016llx", KernelEntry);
	klog("liumos_     %016llx", liumos_);
	klog("n   _       %016llx", &n);

	efi_memory_map_print(liumos_->efi_memory_map);

	//EFI_RuntimeServices *runtime_services = liumos_->loader_info.efi->system_table->runtime_services;
	//runtime_services->reset_system(EfiResetShutdown, 0, 0, NULL);

	Die();
}
