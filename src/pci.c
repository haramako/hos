#include "pci.h"

#include "asm.h"

static inline bool is_aligned(uint64_t n, uint32_t align) { return (n % align) == 0; }

static uint32_t read32_(uint8_t bus, uint8_t slot, uint8_t func, uint16_t offset) {
	assert(is_aligned(offset, 4));
	uint32_t addr = (1UL << 31) | (bus << 16) | (slot << 11) | (func << 8) | offset;
	WriteIOPort32(PCI_IOPORT_ADDR, addr);
	return ReadIOPort32(PCI_IOPORT_DATA);
}

static uint8_t read8_(uint8_t bus, uint8_t slot, uint8_t func, uint16_t offset) {
	uint32_t value = read32_(bus, slot, func, offset & 0xfffc);
	return (value >> ((offset & 0x03) * 8)) & 0xff;
}

static uint16_t read16_(uint8_t bus, uint8_t slot, uint8_t func, uint16_t offset) {
	uint32_t value = read32_(bus, slot, func, offset & 0xfffc);
	return (value >> ((offset & 0x03) * 8)) & 0xffff;
}

static void write32_(uint8_t bus, uint8_t slot, uint8_t func, uint16_t offset, uint32_t value) {
	assert(is_aligned(offset, 4));
	uint32_t addr = (1UL << 31) | (bus << 16) | (slot << 11) | (func << 8) | offset;
	WriteIOPort32(PCI_IOPORT_ADDR, addr);
	WriteIOPort32(PCI_IOPORT_DATA, value);
}

PCI_DeviceInfo *pci_find_device(uint8_t class_code, uint8_t subclass) {
	PCI_Manager *m = &g_pci_manager;
	for (int i = 0; i < m->device_num; i++) {
		PCI_DeviceInfo *info = &m->devices[i];
		if (info->reg.x.class_code == class_code && info->reg.x.subclass == subclass) {
			return info;
		}
	}
	return NULL;
}

static void discover_devices_() {
	PCI_Manager *m = &g_pci_manager;

	// bool pci_find_device(struct pci_device *dev, uint16_t vendor, uint16_t device) {
	for (int bus = 0; bus <= 255; bus++) {
		for (int slot = 0; slot < 32; slot++) {
			for (int func = 0; func < 8; func++) {
				uint16_t vendor2 = read16_(bus, slot, func, PCI_CONFIG_VENDOR_ID);
				if (vendor2 == PCI_VENDOR_NONE) {
					continue;
				}

				PCI_DeviceInfo *d = &m->devices[m->device_num];
				for (int i = 0; i < 16; i++) {
					d->reg.raw[i] = read32_(bus, slot, func, i * sizeof(uint32_t));
				}

				d->bus = bus;
				d->slot = slot;
				d->func = func;
				m->device_num++;
			}
		}
	}
}

void pci_init() { discover_devices_(); }

void pci_print() {
	PCI_Manager *m = &g_pci_manager;

	klog("idx: bus sl fn: vendor dev   cls sub: bar(s)");

	for (int i = 0; i < m->device_num; i++) {
		PCI_DeviceInfo *d = &m->devices[i];
		klog("%3d: %3d %2d %2d:  %5d %5d %3d %3d %d: %p %p %p %p %p %p %p", i, d->bus, d->slot, d->func,
			 d->reg.x.vendor_id, d->reg.x.device_id, d->reg.x.class_code, d->reg.x.subclass, d->reg.x.header_type,
			 (uint64_t)d->reg.x.base_addr0, (uint64_t)d->reg.x.base_addr1, (uint64_t)d->reg.x.base_addr2,
			 (uint64_t)d->reg.x.base_addr3, (uint64_t)d->reg.x.base_addr4, (uint64_t)d->reg.x.base_addr5);
	}
}
