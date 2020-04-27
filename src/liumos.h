#pragma once

#include "common.h"

#include "asm.h"
#include "acpi.h"
#include "efi.h"
#include "hpet.h"
//#include "sys_constant.h"

#define kLAPICRegisterAreaPhysBase 0x00000000FEE00000ULL
#define kLAPICRegisterAreaVirtBase 0xFFFFFFFFFEE00000ULL
#define kLAPICRegisterAreaByteSize 0x0000000000100000ULL

#define kKernelBaseAddr 0xFFFFFFFF00000000ULL

#define kKernelStackPagesForEachProcess 2

// Dummy LuimOS class decralations
typedef void EFIFile;
typedef void ACPI_RSDT;
typedef void ACPI_NFIT;
typedef void ACPI_MADT;
typedef void ACPI_SRAT;
typedef void ACPI_SLIT;
typedef void ACPI_FADT;
typedef void PersistentMemoryManager;
typedef void Sheet;
typedef void Console;
typedef void KeyboardController;
typedef void LocalAPIC;
typedef void CPUFeatureSet;
typedef void PhysicalPageAllocator;
typedef void KernelVirtualHeapAllocator;
typedef void IA_PML4;
typedef void Scheduler;
typedef void ProcessController;
typedef void IDT;
typedef void Process;

// @liumos.c
typedef struct PACKED LoaderInfo {
  struct {
    EFIFile* logo_ppm;
    EFIFile* hello_bin;
    EFIFile* pi_bin;
    EFIFile* liumos_elf;
    EFIFile* liumos_ppm;
  } files;
  EFI* efi;
} LoaderInfo;

#define kNumOfPMEMManagers 4

typedef struct PACKED LiumOS {
  struct {
    ACPI_RSDT* rsdt;
    ACPI_NFIT* nfit;
    ACPI_MADT* madt;
    ACPI_HPET* hpet;
    ACPI_SRAT* srat;
    ACPI_SLIT* slit;
    ACPI_FADT* fadt;
  } acpi;
  LoaderInfo loader_info;
  PersistentMemoryManager* pmem[kNumOfPMEMManagers];
  Sheet* vram_sheet;
  Sheet* screen_sheet;
  Console* main_console;
  KeyboardController* keyboard_ctrl;
  LocalAPIC* bsp_local_apic;
  CPUFeatureSet* cpu_features;
  PhysicalPageAllocator* dram_allocator;
  KernelVirtualHeapAllocator* kernel_heap_allocator;
  HPET* hpet;
  EFI_MemoryMap* efi_memory_map;
  IA_PML4* kernel_pml4;
  Scheduler* scheduler;
  ProcessController* proc_ctrl;
  IDT* idt;
  Process* root_process;
  Process* sub_process;
  uint64_t time_slice_count;
  bool is_multi_task_enabled;
} LiumOS;

extern LiumOS* liumos_;
