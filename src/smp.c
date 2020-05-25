#include "smp.h"

#include "acpi.h"
#include "apic.h"
#include "boot_param.h"
#include "global.h"
#include "physical_memory.h"

static void smp_boot_();

void *find_rsdt_(const char *signature) {
	ACPI_XSDT *xsdt = g_boot_param->rsdt->xsdt;
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
	MPInfo *mp = talloc(MPInfo);

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

#define APIC_LDR 0xd0
#define APIC_ICR_LOW 0x300
#define APIC_ICR_HIGH 0x310
#define APIC_ICR_INIT 0x00000500
#define APIC_ICR_DEST_ALL_EX_SELF 0x000c0000
#define APIC_ICR_STARTUP 0x00000600
#define APIC_ICR_SEND_PENDING 0x00001000

#include "asm.h"
#include "smp_trampoline.h"

typedef struct {
	void (*entry)();
	void *stack;
} SMPBootParam;

static volatile int boot_ack_;

void boot_processor_(int processor_id, void *stack) {
	ktrace("Booting prosessor %d.", processor_id);

	uint8_t *boot_bin = (uint8_t *)0x70000;
	uint64_t bin_size = _trampoline_end - _trampoline;
	memcpy(boot_bin, _trampoline, bin_size);

	// Send IPI.
	// See: https://ja.tech.jar.jp/ac/2018/day15.html

	uint32_t icrl = apic_read_reg(&g_apic, APIC_ICR_LOW);
	uint32_t icrh = apic_read_reg(&g_apic, APIC_ICR_HIGH);

	icrl = (icrl & ~0x000cdfff) | APIC_ICR_INIT;
	icrh = (icrh & 0x000fffff) | (1 << 24);

	apic_write_reg(&g_apic, APIC_ICR_HIGH, icrh);
	apic_write_reg(&g_apic, APIC_ICR_LOW, icrl);

	ktrace("IPI sent.");

	// Send SIPI
	boot_ack_ = 0;

	SMPBootParam *boot_param = (SMPBootParam *)0x8000;
	boot_param->entry = smp_entry;
	boot_param->stack = stack;
	//*entry_addr = 0x4100000000000043;
	// klog("entry_addr %p", *entry_addr);

	do {
		icrl = apic_read_reg(&g_apic, APIC_ICR_LOW);
		icrh = apic_read_reg(&g_apic, APIC_ICR_HIGH);
		/* Wait until it's idle */
	} while (icrl & (APIC_ICR_SEND_PENDING));

	icrl = (icrl & ~0x000cdfff) | APIC_ICR_STARTUP | TRAMPOLINE_VEC;
	icrh = (icrh & 0x000fffff) | (1 << 24);

	apic_write_reg(&g_apic, APIC_ICR_HIGH, icrh);
	apic_write_reg(&g_apic, APIC_ICR_LOW, icrl);

	ktrace("SIPI sent.");

	while (!boot_ack_)
		;

	ktrace("Boot acknowledged processor %d.", processor_id);
}

#include "mutex.h"

atomic_int lock;

volatile int n1;
volatile int n2;

static void smp_boot_() {
	uintptr_t stack = physical_memory_alloc(1);
	boot_processor_(1, (void *)(stack + PAGE_SIZE));

	int i = 0;
	for (;;) {
		spinlock_lock(&lock);
		if ((n1 - n2) != 0) {
			klog("n1 %d %d", n1, n2);
		}
		spinlock_unlock(&lock);
		i++;
		if (i % 1000 == 0) {
			klog("%d", i);
		}
		for (int i = 0; i < 10; i++)
			;
	}
}

void smp_entry() {
	boot_ack_ = 1;
	for (;;) {
		spinlock_lock(&lock);
		n1++;
		for (int i = 0; i < 1000; i++)
			;
		n2++;
		spinlock_unlock(&lock);
		for (int i = 0; i < 1000; i++)
			;
	}
}
