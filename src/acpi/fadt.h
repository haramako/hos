#pragma once

#include "acpi/rsdt.h"

typedef struct PACKED FADT {
	ACPI_SDTHeader h;

	uint32_t firmware_ctrl;
	uint32_t dsdt;
	uint8_t reserved;
	uint8_t preferred_pm_profile;
	uint16_t sci_int;
	uint32_t smi_cmd;
	uint8_t acpi_enable;
	uint8_t acpi_disable;
} FADT;
static_assert(sizeof(FADT) == 54, "Invalid size.");
