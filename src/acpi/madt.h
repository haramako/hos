#pragma once

#include "acpi/rsdt.h"

typedef struct PACKED ACPI_MADT {
	// Multiple APIC Description Table
	ACPI_SDTHeader h;

	uint32_t local_apic_address;
	uint32_t flags;
	uint8_t entries[];
} ACPI_MADT;

typedef enum ACPI_MADTEntryType {
	ACPI_MADT_ENTRY_PROCESSOR_LOCAL_APIC = 0x0,
	ACPI_MADT_ENTRY_IO_APIC = 0x1,
	ACPI_MADT_ENTRY_INTERRUPT_SOURCE_OVERRIDE = 0x2,
	ACPI_MADT_ENTRY_LOCAL_APIC_NMI = 0x4,
} ACPI_MADTEntryType;

typedef struct PACKED {
	uint8_t processor_id;
	uint8_t apic_id;
	uint32_t apic_flags;
} ACPI_MADT_ProcessorLocalApic;

typedef struct PACKED {
	uint8_t io_apic_id;
	uint8_t dummy;
	uint32_t io_apic_address;
	uint32_t global_system_interrupt_base;
} ACPI_MADT_IOApic;

typedef struct PACKED {
	uint8_t bus_source;
	uint8_t irq_source;
	uint32_t global_system_interrupt;
	uint16_t flags;
} ACPI_MADT_InterruptSourceOverride;

typedef struct PACKED {
	uint8_t processor_id;
	uint16_t flags;
	uint8_t lint;
} ACPI_MADT_LocalAPIC_NMI;
