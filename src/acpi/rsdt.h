#pragma once

#include "common.h"

//#include "apic.h"
#include "efi/guid.h"

#define kDescriptionHeaderSize 36

typedef struct PACKED ACPI_RSDT_ {
	char signature[8];
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t length;
	struct ACPI_XSDT_ *xsdt;
	uint8_t extended_checksum;
	uint8_t reserved;
} ACPI_RSDT;

typedef struct PACKED ACPI_SDTHeader_ {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} ACPI_SDTHeader;

typedef struct PACKED ACPI_XSDT_ {
	ACPI_SDTHeader h;

	struct ACPI_SDTHeader_ *entry[];
} ACPI_XSDT;
