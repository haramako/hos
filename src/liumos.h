#pragma once

#include "common.h"

#include "acpi.h"
#include "efi.h"

// @liumos.c
typedef struct PACKED LoaderInfo_ {
	struct {
		void *logo_ppm;
		void *hello_bin;
		void *pi_bin;
		void *liumos_elf;
		void *liumos_ppm;
	} files;
	EFI *efi;
} LoaderInfo;

#define kNumOfPMEMManagers 4

typedef struct PACKED LiumOS_ {
	struct {
		ACPI_RSDT *rsdt;
		void *nfit;
		void *madt;
		ACPI_HPET *hpet;
		void *srat;
		void *slit;
		void *fadt;
	} acpi;
	LoaderInfo loader_info;
	void *pmem[kNumOfPMEMManagers];
	void *vram_sheet;
	void *screen_sheet;
	void *main_console;
	void *keyboard_ctrl;
	void *bsp_local_apic;
	struct CPUFeatureSet *cpu_features;
	void *dram_allocator;
	void *kernel_heap_allocator;
	void *hpet;
	EFI_MemoryMap *efi_memory_map;
	void *kernel_pml4;
	/*Scheduler*/ void *scheduler;
	/*ProcessController*/ void *proc_ctrl;
	void *idt;
	void *root_process;
	void *sub_process;
	uint64_t time_slice_count;
	bool is_multi_task_enabled;
} LiumOS;

extern LiumOS *g_liumos;
