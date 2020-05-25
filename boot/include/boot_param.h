#pragma once

#include <stdint.h>

typedef struct {
	struct ACPI_RSDT_ *rsdt;
	struct Sheet_ *vram_sheet;
	// struct CPUFeatureSet *cpu_features;
	struct EFI_MemoryMap_ *efi_memory_map;
} BootParam;
