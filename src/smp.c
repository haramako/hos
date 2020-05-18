#include "smp.h"

#include "acpi.h"
#include "apic.h"
#include "liumos.h"

static void smp_boot_();

void *find_rsdt_(const char *signature) {
	ACPI_XSDT *xsdt = g_liumos->acpi.rsdt->xsdt;
	const int rsdt_num = (xsdt->length - kDescriptionHeaderSize) >> 3;
	for (int i = 0; i < rsdt_num; i++) {
		if (strncmp(xsdt->entry[i]->signature, signature, 4) == 0) {
			return (void *)xsdt->entry[i];
		}
	}
	return NULL;
}

typedef struct {
	uint8_t id;
	uint8_t apic_id;
	uint32_t apic_flags;
	void *local_apic;
} ProcessorInfo;

#define MAX_PROCESSOR 32

typedef struct {
	ProcessorInfo processors[MAX_PROCESSOR];
} MPInfo;

void smp_init() {
	MPInfo *mp = kalloc(MPInfo);

	ACPI_MADT *madt = find_rsdt_("APIC");
	kcheck0(madt);

	uint8_t *entry = madt->entries;
	uint8_t *entry_end = (uint8_t *)madt + madt->h.length;
	while (entry < entry_end) {
		uint8_t type = entry[0];
		uint8_t len = entry[1];
		klog("type=%d, len=%d", type, len);
		switch (type) {
		case ACPI_MADT_ENTRY_PROCESSOR_LOCAL_APIC: {
			ACPI_MADT_ProcessorLocalApic *e = (ACPI_MADT_ProcessorLocalApic *)(entry + 2);
			uint8_t id = e->processor_id;
			mp->processors[id].apic_id = e->apic_id;
			mp->processors[id].apic_flags = e->apic_flags;
		} break;
		}
		entry += len;
	}
	for (int i = 0; i < MAX_PROCESSOR; i++) {
		ProcessorInfo *p = &mp->processors[i];
		if (p->apic_flags == 0) continue;
		klog("%d: apic_id=%d, apic_flags=%d", i, p->apic_id, p->apic_flags);
	}

	smp_boot_();
}

#define APIC_ICR_LOW 0x300
#define APIC_ICR_HIGH 0x310
#define APIC_ICR_INIT 0x00000500
#define APIC_ICR_DEST_ALL_EX_SELF 0x000c0000
#define APIC_ICR_STARTUP 0x00000600
#define APIC_ICR_SEND_PENDING 0x00001000

extern uint8_t _binary_smp_trampoline_raw_start[];
extern uint8_t _binary_smp_trampoline_raw_size[];

#include "asm.h"

static void smp_boot_() {
	// Send IPI.
	// See: https://ja.tech.jar.jp/ac/2018/day15.html

	klog("CR3 %016lx", ReadCR3());

	uint8_t *boot_bin = (uint8_t *)0x70000;
	uint64_t bin_size = (uint64_t)_binary_smp_trampoline_raw_size;
	klog("bin_size %lld", bin_size);
	memcpy(boot_bin, _binary_smp_trampoline_raw_start, bin_size);

	uint32_t icrl;
	uint32_t icrh;
	volatile uint8_t *apic_base;

	apic_base = (volatile uint8_t *)g_apic.base_addr;

	icrl = *(volatile uint32_t *)(apic_base + APIC_ICR_LOW);
	icrh = *(volatile uint32_t *)(apic_base + APIC_ICR_HIGH);

	klog("icr %x %x", icrh, icrl);

	icrl = (icrl & ~0x000cdfff) | APIC_ICR_INIT | APIC_ICR_DEST_ALL_EX_SELF;
	icrh = (icrh & 0x000fffff);

	*(volatile uint32_t *)(apic_base + APIC_ICR_HIGH) = icrh;
	*(volatile uint32_t *)(apic_base + APIC_ICR_LOW) = icrl;

	klog("icr %x %x", icrh, icrl);

	klog("init apic");

	// Send SIPI

	do {
		icrl = *(volatile uint32_t *)(apic_base + APIC_ICR_LOW);
		icrh = *(volatile uint32_t *)(apic_base + APIC_ICR_HIGH);
		/* Wait until it's idle */
	} while (icrl & (APIC_ICR_SEND_PENDING));

	const int vector = 0x70;
	icrl = (icrl & ~0x000cdfff) | APIC_ICR_STARTUP | APIC_ICR_DEST_ALL_EX_SELF | vector;
	icrh = (icrh & 0x000fffff);

	*(volatile uint32_t *)(apic_base + APIC_ICR_HIGH) = icrh;
	*(volatile uint32_t *)(apic_base + APIC_ICR_LOW) = icrl;
}

void smp_entry() {
	for (;;)
		;
}
