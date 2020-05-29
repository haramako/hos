#pragma once

#include "acpi/rsdt.h"

// SLIT: System Locality Distance Information
typedef struct PACKED SLIT {
	ACPI_SDTHeader h;

	uint64_t num_of_system_localities;
	uint8_t entry[1];
} SLIT;
static_assert(offsetof(SLIT, entry) == 44, "Invalid size.");
