// Test code, included directory from main.c.
#include <elf.h>

#include <string.h>

#include "fat.h"
#include "mm.h"

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

// FAT test
extern uint8_t _binary_fat_test_test_fat_start[];
extern uint8_t _binary_fat_test_test_fat_size[];

typedef struct {
	uint8_t *fs_bin;
	uint64_t fs_size;
} FAT_UserData;

#define FAT_SECTOR_SIZE 512

static void blk_read_(void *data, size_t sector, void *buf, size_t num_sectors) {
	FAT_UserData *d = (FAT_UserData *)data;
	memcpy(buf, d->fs_bin + sector * FAT_SECTOR_SIZE, num_sectors * FAT_SECTOR_SIZE);
}

static void blk_write_(void *data, size_t offset, const void *buf, size_t len) { kpanic("NYI"); }

static struct fat fs;

static void bootfs_init() {
	FAT_UserData *data = malloc(sizeof(FAT_UserData));
	data->fs_bin = _binary_fat_test_test_fat_start;
	data->fs_size = (uint64_t)_binary_fat_test_test_fat_size;

	error_t err = fat_probe(&fs, blk_read_, blk_write_, data);
	assert(err == OK);
}

static void fat_test_() {
	error_t err;
	struct fat_dir dir;
	struct fat_dirent *e;
	char tmp[12];
	kcheck(fat_opendir(&fs, &dir, "/") == OK, "fat_opendir failed!");
	while ((e = fat_readdir(&fs, &dir)) != NULL) {
		strncpy(tmp, (const char *)e->name, sizeof(tmp));
		tmp[11] = '\0';
		klog("/%s", tmp);

		char buf[512];
		struct fat_file file;
		err = fat_open(&fs, &file, tmp);
		kcheck(err == OK, "fat_open failed!");
		err = fat_read(&fs, &file, 0, buf, MIN(file.size, 8));
		kcheck(err == OK, "fat_read failed!");
		buf[8] = '\0';
		klog("%s", buf);
	}
}

// Process test 2

typedef void (*EntryPoint)();

static void process_test2_() {

	char *bin = (char *)0x0000100000000000ULL;
	PageMapEntry *new_pml4 = page_copy_page_map_table((PageMapEntry *)ReadCR3());
	MemoryMap *mm = mm_new();
	PageAttribute attr = {.is_user = true};
	mm_alloc(mm, bin, 4, &attr);
	page_pme_alloc_addr(new_pml4, bin, 4, true, true);

	char *buf = (char *)physical_memory_alloc(10);

	struct fat_file file;
	error_t err = fat_open(&fs, &file, "HELLO   ELF");
	kcheck(err == OK, "fat_open failed!");
	err = fat_read(&fs, &file, 0, buf, file.size);
	kcheck(err == OK, "fat_read failed!");
	Elf64_Ehdr *elf = (Elf64_Ehdr *)buf;

	klog("H");
	page_memcpy(new_pml4, bin, buf, file.size);

	{
		EntryPoint entry_point = (EntryPoint)elf->e_entry;
		// klog("size %lld, ep = %p, entry_point = %p", bin_size, elf->e_entry, entry_point);

		ProcessCreateParam param = {.entry_point = entry_point, .pml4 = new_pml4};
		Process *p = process_create(&param);
		process_print(p);
		pme_print(new_pml4);
	}
}

// Paging test.
static void paging_test_() {
	uint64_t cr0 = asm_read_cr0();
	uint64_t cr3 = ReadCR3();
	klog("CR0 %p", cr0);
	klog("CR3 %p", cr3);

	// int n = *((int*)0x01);
	//*((int*)0x01) = 1;

	int *x = (int *)0x0000000080000000;
	// int * x = (int*)0x000000007f000000;
	// int * x = (int*)0x000000007fffffff;
	// int * x = (int*)0x00000000ffe00000;
	klog("%d", *x);
	*x = 1;
	klog("%d", *x);

	PageMapEntry *pml4 = (PageMapEntry *)cr3;

	void *p = (void *)(7 * 1024 * 1024 + 123);
	klog("v2p %p => %p", p, page_v2p(pml4, p));

	pme_print(pml4);

	PageMapEntry *new_pml4 = page_copy_page_map_table(pml4);

	klog("=================");
	pme_print(new_pml4);

	{
		int *x2 = (int *)0xffff800000000100;
		page_alloc_addr((void *)((uint64_t)x2 & ~(PAGE_SIZE - 1)), 100, false, false);
		*x2 = 1;

		klog("=================");
		pme_print(pml4);
	}

	{
		int *x2 = (int *)0xffff800000010000;
		*x2 = 1;

		klog("=================");
		pme_print(pml4);
	}
}

// MemoryMap test.
void mm_test_() {
	MemoryMap *mm = mm_new();
	PageAttribute attr = {.is_user = true};
	MemoryBlock *b1 = mm_alloc(mm, (void *)0x0000200000000000, 1, &attr);

	MemoryBlock *b2 = mm_alloc(mm, (void *)0x0000300000000000, 8, &attr);

	mm_print(mm);

	{
		MemoryBlock *b = mm_find_vaddr(mm, (void *)0x0000200000000000);
		kcheck(b == b1, "unexpect block");
		kcheck(b && b->vaddr_start == 0x0000200000000000, "unexpect vaddr_start.");
		kcheck(b && b->vaddr_end == 0x0000200000001000, "unexpect vaddr_end.");
	}

	{
		MemoryBlock *b = mm_find_vaddr(mm, (void *)0x0000200000000fff);
		kcheck(b == b1, "unexpect block");
	}

	{
		MemoryBlock *b = mm_find_vaddr(mm, (void *)0x0000200000001000);
		kcheck(b == NULL, "unexpect block");
	}

	{
		MemoryBlock *b = mm_find_vaddr(mm, (void *)0x0000300000000000);
		kcheck0(b == b2);
	}

	{
		mm_alloc(g_kernel_mm, (void *)0x0000200000000000, 1, &attr);
		int *p = (int *)0x0000200000000000;
		//*p = 1;
	}
}
