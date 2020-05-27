#pragma once

#include "acpi/rsdt.h"

#define kTypeLAPICAffinity 0x00
#define kTypeMemoryAffinity 0x01
#define kTypeLx2APICAffinity 0x02

typedef struct PACKED SRAT_Entry {
	uint8_t type;
	uint8_t length;
} SRAT_Entry;

typedef struct PACKED SRAT_LAPICAffinity {
	SRAT_Entry entry;
	uint8_t proximity_domain_low;
	uint8_t apic_id;
	uint32_t flags;
	uint8_t local_sapic_eid;
	uint8_t proximity_domain_high[3];
	uint32_t clock_domain;
} SRAT_LAPICAffinity;
static_assert(sizeof(SRAT_LAPICAffinity) == 16, "Invalid size.");

typedef struct PACKED SRAT_MemoryAffinity {
	SRAT_Entry entry;
	uint32_t proximity_domain;
	uint16_t reserved0;
	uint64_t base_address;
	uint64_t size;
	uint32_t reserved1;
	uint32_t flags;
	uint64_t reserved2;
} SRAT_MemoryAffinity;
static_assert(sizeof(SRAT_MemoryAffinity) == 40, "Invalid size.");

typedef struct PACKED SRAT_Lx2APICAffinity {
	SRAT_Entry entry;
	uint16_t reserved0;
	uint32_t proximity_domain;
	uint32_t x2apic_id;
	uint32_t flags;
	uint32_t clock_domain;
	uint32_t reserved1;
} SRAT_Lx2APICAffinity;
static_assert(sizeof(SRAT_Lx2APICAffinity) == 24, "Invalid size.");

typedef struct PACKED SRAT {
	ACPI_SDTHeader h;
	uint32_t reserved[3];
	SRAT_Entry entry[1];
} SRAT;
static_assert(offsetof(SRAT, entry) == 48, "Invalid size.");
