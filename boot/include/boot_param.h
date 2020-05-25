#pragma once

#include <stdint.h>

typedef struct LiumOS_ {
	struct {
		struct ACPI_RSDT_ *rsdt;
		struct ACPI_HPET_ *hpet;
	} acpi;
	struct Sheet_ *vram_sheet;
	struct CPUFeatureSet *cpu_features;
	struct EFI_MemoryMap_ *efi_memory_map;
} LiumOS;

extern LiumOS *g_liumos;
