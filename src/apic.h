#pragma once

#include "common.h"

#define kRegisterAreaSize 0x400

typedef struct PACKED LocalAPIC_ {
	uint64_t base_addr;
	uint64_t kernel_virt_base_addr;
	uint32_t id;
	bool is_x2apic;
} LocalAPIC;

extern LocalAPIC g_apic;

void apic_init();
void apic_new(LocalAPIC *a);
inline uint32_t apic_get_id(LocalAPIC *a) { return a->id; }
inline bool apic_is_x2(LocalAPIC *a) { return a->is_x2apic; }
void apic_send_end_of_interrupt(LocalAPIC *a);
void apic_print();

inline uint32_t apic_read_reg(LocalAPIC *a, uint64_t offset) {
	return *((uint32_t *)(a->kernel_virt_base_addr + offset));
}

inline void apic_write_reg(LocalAPIC *a, uint64_t offset, uint32_t data) {
	*((uint32_t *)(a->kernel_virt_base_addr + offset)) = data;
}

inline uint32_t *apic_get_register_addr(LocalAPIC *a, uint64_t offset) { return (uint32_t *)(a->base_addr + offset); }
