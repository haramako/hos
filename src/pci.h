#pragma once

#include "common.h"

#define PCI_IOPORT_ADDR 0x0cf8
#define PCI_IOPORT_DATA 0x0cfc
#define PCI_ANY 0
#define PCI_VENDOR_NONE 0xffff

#define PCI_CONFIG_VENDOR_ID 0x00
#define PCI_CONFIG_DEVICE_ID 0x02
#define PCI_CONFIG_COMMAND 0x04
#define PCI_CONFIG_BAR0 0x10
#define PCI_CONFIG_INTR_LINE 0x3c

typedef enum {
	PCI_CLASS_CODE_NONE,
	PCI_CLASS_CODE_MASS_STORAGE_CONTROLLER,
	PCI_CLASS_CODE_BRIDGE_DEVICE = 6,
} PCI_ClassCode;

typedef struct {
	uint8_t bus;
	uint8_t slot;
	uint8_t func;
	union {
		uint32_t raw[16];
		struct PACKED {
			// 0
			uint16_t vendor_id;
			uint16_t device_id;

			// 1
			uint16_t command;
			uint16_t status;

			// 2
			uint8_t revision_id;
			uint8_t prog_if;
			uint8_t subclass;
			uint8_t class_code;

			// 3
			uint8_t cache_line_size;
			uint8_t latency_timer;
			uint8_t header_type;
			uint8_t bist;

			// 4~5
			uint32_t base_addr0;
			uint32_t base_addr1;

			// 6~10
			uint32_t base_addr2;
			uint32_t base_addr3;
			uint32_t base_addr4;
			uint32_t base_addr5;
			uint32_t cardbus_cis_pointer;

			// 11
			uint16_t subsystem_vendor_id;
			uint16_t subsystem_id;

			// 12
			uint32_t expantion_rom_base_addr;

			// 13
			uint8_t capabilities_pointer;
			uint32_t dummy0 : 24;

			// 14
			uint32_t dummy1;

			// 15
			uint8_t interrupt_line;
			uint8_t interrupt_pin;
			uint8_t min_grant;
			uint8_t max_latency;

#if 0
			// 6
			uint8_t primary_bus_number;
			uint8_t secondary_bus_number;
			uint8_t subordinate_bus_number;
			uint8_t secondary_latency_timer;

			// 7
			uint8_t io_base;
			uint8_t io_limit;
			uint16_t secondary_status;

			// 8
			uint16_t memory_base;
			uint16_t memory_limit;

			// 9
			uint16_t prefechable_memory_base;
			uint16_t prefechable_memory_limit;

			// 10~11
			uint32_t prefechable_base_upper;
			uint32_t prefechable_limit_upper;

			// 12
			uint16_t io_base_upper;
			uint16_t io_limit_upper;

			// 13
			uint8_t capability_pointer;
			uint32_t dummy1:24;

			// 14
			uint32_t expansion_rom_base_address;

			// 15
			uint8_t interrupt_line;
			uint8_t interrupt_pin;
			uint16_t bride_control;
#endif

#if 0
			// 4
			uint32_t cardbus_addr;

			// 5
			uint8_t offset_of_capabilities_list;
			uint8_t dummy0;
			uint16_t secondary_status;

			// 6
			uint8_t pci_bus_number;
			uint8_t cardbus_bus_number;
			uint8_t subordinate_bus_number;
			uint8_t cardbus_latency_timer;

			// 7~14
			uint32_t memory_base_addr0;
			uint32_t memory_limit0;
			uint32_t memory_base_addr1;
			uint32_t memory_limit1;
			uint32_t io_base_addr0;
			uint32_t io_limit0;
			uint32_t io_base_addr1;
			uint32_t io_limit1;

			// 15
			uint8_t interrupt_line;
			uint8_t interrupt_pin;
			uint16_t bridge_control;

			// 16
			uint16_t subsystem_device_id;
			uint16_t subsystem_vendor_id;

			// 17
			uint32_t pccard_lagacy_mode_base_addr;
#endif
		} x;
	} reg;
} PCI_DeviceInfo;

typedef struct {
	PCI_DeviceInfo devices[256];
	int device_num;
} PCI_Manager;

PCI_Manager g_pci_manager;

void pci_init();
void pci_print();
PCI_DeviceInfo *pci_find_device(uint8_t class_code, uint8_t subclass);
