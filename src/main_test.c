// Test code, included directory from main.c.

static void test_malloc_() {
	int *a = (int *)malloc(4);
	int *b = (int *)malloc(4);
	free(a);
	int *c = (int *)malloc(4);
	int n = 99;

	klog("a = %p\nb= %p\nc= %p", (void *)a, (void *)b, c);

	klog("KernelEntry %016llx", hpet_init);
	klog("g_liumos     %016llx", g_liumos);
	klog("n   _       %016llx", &n);
}

static void test_virtual_memory_map_() {
	EFI_RuntimeServices *runtime_services = g_liumos->loader_info.efi->system_table->runtime_services;
	EFI_MemoryMap *mm = g_liumos->efi_memory_map;
	int res = runtime_services->set_virtual_address_map(sizeof(mm->buf), mm->descriptor_size, mm->key, (void *)mm->buf);
	klog("res = %d", res);
	efi_memory_map_print(g_liumos->efi_memory_map);
}

static void test_memory_map_() { efi_memory_map_print(g_liumos->efi_memory_map); }

static void test_reset_() {
	EFI_RuntimeServices *runtime_services = g_liumos->loader_info.efi->system_table->runtime_services;
	runtime_services->reset_system(EfiResetShutdown, 0, 0, NULL);
}

// Timer test
static void timer_test_callback1_(TimerParam *p, void *data) { klog("call_periodic"); }

static void timer_test_callback2_(TimerParam *p, void *data) { klog("call_after"); }

static void timer_test_() {
	timer_call_periodic(1 * SEC, timer_test_callback1_, NULL);
	timer_call_after(3 * SEC, timer_test_callback2_, NULL);
	timer_print();
	for (;;) {
		hpet_busy_wait(1000);
		console_write(".");
	}
}

// Process test
static void process_test_process1_() {
	klog("[1]");
	for (;;) {
		hpet_busy_wait(1000);
		console_write("1");
	}
}

static void process_test_process2_() {
	klog("[2]");
	for (;;) {
		hpet_busy_wait(1000);
		console_write("2");
	}
}

static void process_test_() {
	{
		ProcessCreateParam param = {.entry_point = process_test_process1_};
		Process *p = process_create(&param);
		process_print(p);
	}
	{
		ProcessCreateParam param = {.entry_point = process_test_process2_};
		Process *p = process_create(&param);
		process_print(p);
	}

	for (;;) {
		hpet_busy_wait(1000);
		console_write(".");
	}
}

extern uint8_t _binary_hello_elf_start[];
extern uint8_t _binary_hello_elf_end[];
extern uint8_t _binary_hello_elf_size[];

#include "page.h"
#include <elf.h>

typedef void (*EntryPoint)();

// Process test
static void process_test2_() {
	uint8_t *bin_start = _binary_hello_elf_start;
	uint64_t bin_size = (uint64_t)_binary_hello_elf_size;
	Elf64_Ehdr *elf = (Elf64_Ehdr *)bin_start;

	char *bin = (char *)0x0000100000000000ULL;
	page_alloc_addr(bin, 4, true);
	memcpy(bin, bin_start, bin_size);

	{
		EntryPoint entry_point = (EntryPoint)elf->e_entry;
		// klog("size %lld, ep = %p, entry_point = %p", bin_size, elf->e_entry, entry_point);

		ProcessCreateParam param = {.entry_point = entry_point};
		Process *p = process_create(&param);
		// process_print(p);
		// pme_print((PageMapEntry *)ReadCR3());
	}
}
