#include "liumos.h"

#include "asm.h"
#include "console.h"
#include "serial.h"

static Serial com1_mem_;
static Serial* com1_ = &com1_mem_;

LiumOS *liumos_;

void KernelEntry(LiumOS* liumos_passed)
{
	int *a = (int*)malloc(4);
	int *b = (int*)malloc(4);
	int n = 99;
	
	liumos_ = liumos_passed;
	
	serial_init(com1_, 0x2f8);
	console_init(com1_);

	klog("\n=======================");
	klog("Kernel start");

	klog("hoge %p\nFUGA %p", (void*)a, (void*)b);

	klog("KernelEntry %016llx", KernelEntry);
	klog("liumos_     %016llx", liumos_);
	klog("com1_       %016llx", com1_);
	klog("n   _       %016llx", &n);

	efi_memory_map_print(liumos_->efi_memory_map);

	//EFI_RuntimeServices *runtime_services = liumos_->loader_info.efi->system_table->runtime_services;
	//runtime_services->reset_system(EfiResetShutdown, 0, 0, NULL);

	Die();
}
