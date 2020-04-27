#pragma once

#include "common.h"

#include "apic.h"
#include "guid.h"

#define kDescriptionHeaderSize 36

typedef struct PACKED RSDT {
	char signature[8];
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t length;
	struct XSDT* xsdt;
	uint8_t extended_checksum;
	uint8_t reserved;
} RSDT;

typedef struct PACKED XSDT {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
	void* entry[1];
} XSDT;

#define kTypeSPARangeStructure 0x00
#define kTypeNVDIMMRegionMappingStructure 0x01
#define kTypeInterleaveStructure 0x02
#define kTypeNVDIMMControlRegionStructure 0x04
#define kTypeFlushHintAddressStructure 0x06
#define kTypePlatformCapabilitiesStructure 0x07

typedef struct PACKED NFIT_Entry {
    uint16_t type;
    uint16_t length;
} NFIT_Entry;

// System Physical Address Range
const GUID NIFT_SPARange_kByteAdressablePersistentMemory;
const GUID NIFT_SPARange_kNVDIMMControlRegion;

typedef struct PACKED NIFT_SPARange {
    uint16_t type;
    uint16_t length;
    uint16_t spa_range_structure_index;
    uint16_t flags;
    uint32_t reserved;
    uint32_t proximity_domain;
    uint64_t address_range_type_guid[2];
    uint64_t system_physical_address_range_base;
    uint64_t system_physical_address_range_length;
    uint64_t address_range_memory_mapping_attribute;
} NFIT_SPARange;

typedef struct PACKED NFIT_RegionMapping {
    uint16_t type;
    uint16_t length;
    uint32_t nfit_device_handle;
    uint16_t nvdimm_physical_id;
    uint16_t nvdimm_region_id;
    uint16_t spa_range_structure_index;
    uint16_t nvdimm_control_region_struct_index;
    uint64_t nvdimm_region_size;
    uint64_t region_offset;
    uint64_t nvdimm_physical_address_region_base;
    uint16_t interleave_structure_index;
    uint16_t interleave_ways;
    uint16_t nvdimm_state_flags;
    uint16_t reserved;
} NFIT_RegionMapping;
static_assert(sizeof(NFIT_RegionMapping) == 48, "Invalid size.");

typedef struct PACKED NFIT_InterleaveStructure {
    uint16_t type;
    uint16_t length;
    uint16_t interleave_struct_index;
    uint16_t reserved;
    uint32_t num_of_lines_described;
    uint32_t line_size;
    uint32_t line_offsets[1];
} NFIT_InterleaveStructure;
static_assert(offsetof(NFIT_InterleaveStructure, line_offsets) == 16, "Invalid size.");

typedef struct PACKED NFIT_FlushHintAddressStructure {
    uint16_t type;
    uint16_t length;
    uint32_t nfit_device_handle;
    uint16_t num_of_flush_hint_addresses;
    uint16_t reserved[3];
    uint64_t flush_hint_addresses[1];
} NFIT_FlushHintAddressStructure;
static_assert(offsetof(NFIT_FlushHintAddressStructure, flush_hint_addresses) == 16, "Invalid size.");

typedef struct PACKED NFIT_PlatformCapabilities {
    uint16_t type;
    uint16_t length;
    uint8_t highest_valid_cap_bit;
    uint8_t reserved0[3];
    uint32_t capabilities;
    uint32_t reserved1;
} NFIT_PlatformCapabilities;
static_assert(sizeof(NFIT_PlatformCapabilities) == 16, "Invalid size.");

typedef struct PACKED NFIT_ControlRegionStruct {
    uint16_t type;
    uint16_t length;
    uint16_t nvdimm_control_region_struct_index;
} NFIT_ControlRegionStruct;

typedef struct PACKED NFIT {

	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
	uint32_t reserved;
	NFIT_Entry entry[1];
} NFIT;
static_assert(offsetof(NFIT, entry) == 40, "Invalid size.");

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

	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
	uint32_t reserved[3];
	SRAT_Entry entry[1];

#if 0
	constexpr static uint32_t kUnknownProximityDomain = 0xffffffffULL;
	uint32_t GetProximityDomainForAddrRange(uint64_t paddr, uint64_t size) {
		for (auto& it : *this) {
			if (it.type != Entry::kTypeMemoryAffinity)
				continue;
			MemoryAffinity* e = reinterpret_cast<MemoryAffinity*>(&it);
			if (e->base_address <= paddr && paddr < e->base_address + e->size &&
				paddr + size <= e->base_address + e->size) {
				return e->proximity_domain;
			}
		}
		return kUnknownProximityDomain;
	}
	uint32_t GetProximityDomainForLocalAPIC(LocalAPIC & lapic) {
		for (auto& it : *this) {
			if (it.type != SRAT::Entry::kTypeLx2APICAffinity)
				continue;
			SRAT::Lx2APICAffinity* e = reinterpret_cast<SRAT::Lx2APICAffinity*>(&it);
			if (e->x2apic_id != lapic.GetID())
				continue;
			return e->proximity_domain;
		}
		for (auto& it : *this) {
			if (it.type != SRAT::Entry::kTypeLAPICAffinity)
				continue;
			SRAT::LAPICAffinity* e = reinterpret_cast<SRAT::LAPICAffinity*>(&it);
			if (e->apic_id != lapic.GetID())
				continue;
			return e->proximity_domain_low | (e->proximity_domain_high[0] << 8) |
				(e->proximity_domain_high[1] << 16) |
				(e->proximity_domain_high[2] << 24);
		}
		return kUnknownProximityDomain;
	}
#endif
} SRAT;
static_assert(offsetof(SRAT, entry) == 48, "Invalid size.");

typedef struct PACKED SLIT {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
	uint64_t num_of_system_localities;
	uint8_t entry[1];
} SLIT;
static_assert(offsetof(SLIT, entry) == 44, "Invalid size.");

typedef struct PACKED GAS {
	// Generic Address Structure
	uint8_t address_space_id;
	uint8_t register_bit_width;
	uint8_t register_bit_offset;
	uint8_t reserved;
	void* address;
} GAS;
static_assert(sizeof(GAS) == 12, "Invalid size.");

typedef struct PACKED ACPI_HPET {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;

	uint32_t event_timer_block_id;
	GAS base_address;
	uint8_t hpet_number;
	uint16_t main_counter_minimum_clock_tick_in_periodic_mode;
	uint8_t page_protection_and_oem_attribute;
} ACPI_HPET;

typedef enum MADTStructureType {
	kProcessorLocalAPICInfo = 0x0,
	kIOAPICInfo = 0x1,
	kInterruptSourceOverrideInfo = 0x2,
	kLocalAPICNMIStruct = 0x4,
	kProcessorLocalx2APICStruct = 0x9,
	kLocalx2APICNMIStruct = 0xA,
} MADTStructureType;

typedef struct PACKED MADT {
	// Multiple APIC Description Table
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;

	uint32_t local_apic_address;
	uint32_t flags;
	uint8_t entries[1];
} MADT;

typedef struct PACKED FADT {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint64_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;

	uint32_t firmware_ctrl;
	uint32_t dsdt;
	uint8_t reserved;
	uint8_t preferred_pm_profile;
	uint16_t sci_int;
	uint32_t smi_cmd;
	uint8_t acpi_enable;
	uint8_t acpi_disable;

	#if 0
	uint16_t GetResetReg() {
		return reinterpret_cast<uint64_t>(
										  reinterpret_cast<GAS*>(reinterpret_cast<uint64_t>(this) + 116)
										  ->address);
	}
	uint8_t GetResetValue() { return reinterpret_cast<uint8_t*>(this)[128]; }
	#endif
} FADT;
static_assert(sizeof(FADT) == 54, "Invalid size.");

#if 0
void DetectTables();
#endif
