#pragma once

#include <stdint.h>

#define kBufferSize 0x8000

typedef struct EFI_MemoryMap_ {
	uintptr_t key;
	uintptr_t bytes_used;
	uintptr_t descriptor_size;
	uint32_t descriptor_version;
	uint8_t buf[kBufferSize];
} EFI_MemoryMap;

typedef struct {
	struct ACPI_RSDT_ *rsdt;
	struct Sheet_ *vram_sheet;
	// struct CPUFeatureSet *cpu_features;
	EFI_MemoryMap *efi_memory_map;
} BootParam;
