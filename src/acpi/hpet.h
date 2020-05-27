#pragma once

#include "acpi/rsdt.h"

typedef struct PACKED {
	// Generic Address Structure
	uint8_t address_space_id;
	uint8_t register_bit_width;
	uint8_t register_bit_offset;
	uint8_t reserved;
	void *address;
} ACPI_HPET_GAS;
static_assert(sizeof(ACPI_HPET_GAS) == 12, "Invalid size.");

typedef struct PACKED ACPI_HPET_ {
	ACPI_SDTHeader h;

	uint32_t event_timer_block_id;
	ACPI_HPET_GAS base_address;
	uint8_t hpet_number;
	uint16_t main_counter_minimum_clock_tick_in_periodic_mode;
	uint8_t page_protection_and_oem_attribute;
} ACPI_HPET;
