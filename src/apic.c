#include "apic.h"

#include "asm.h"
#include "console.h"
#include "liumos.h"

LocalAPIC g_apic;

extern inline uint32_t apic_read_register(LocalAPIC *a, uint64_t offset);
extern inline void apic_write_register(LocalAPIC *a, uint64_t offset, uint32_t data);
extern inline uint32_t* apic_get_register_addr(LocalAPIC *a, uint64_t offset);


static void Panic(const char* msg)
{
}

void apic_new(LocalAPIC *a)
{
	uint64_t base_msr = ReadMSR(MSRIndex_kLocalAPICBase);

	if (!(base_msr & kLocalAPICBaseBitAPICEnabled)) Panic("APIC not enabled");

	if (g_liumos->cpu_features->x2apic &&
		!(base_msr & kLocalAPICBaseBitx2APICEnabled)) {
		base_msr |= kLocalAPICBaseBitx2APICEnabled;
		WriteMSR(MSRIndex_kLocalAPICBase, base_msr);
		base_msr = ReadMSR(MSRIndex_kLocalAPICBase);
	}

	#if 0
	a->base_addr = (base_msr & GetPhysAddrMask()) & ~0xfffULL;
	a->kernel_virt_base_addr = liumos->kernel_heap_allocator->MapPages<uint64_t>(
																			   base_addr_, ByteSizeToPageSize(kRegisterAreaSize),
																			   kPageAttrPresent | kPageAttrWritable | kPageAttrWriteThrough |
																			   kPageAttrCacheDisable);
	#endif

	ktrace("LocalAPIC at 0x%016llx", a->base_addr);
	ktrace(" is mapped at 0x%016llx", a->kernel_virt_base_addr);
	a->is_x2apic = (base_msr & (1 << 10));
	ktrace(" in kernel.\nmode: %s", (a->is_x2apic ? "x2APIC" : "xAPIC"));

	#if 0
	CPUID cpuid;
	ReadCPUID(&cpuid, CPUIDIndex::kXTopology, 0);
	id_ = cpuid.edx;
	PutStringAndHex(" id", id_);
	liumos->bsp_local_apic = this;
	#endif
}

void apic_send_end_of_interrupt(LocalAPIC *a)
{
	if (a->is_x2apic) {
		// WRMSR of a non-zero value causes #GP(0).
		WriteMSR(MSRIndex_kx2APICEndOfInterrupt, 0);
		return;
	}
	apic_write_register(a, 0xB0ULL, 0);
}

static uint32_t read_io_apic_register_(LocalAPIC *a, uint8_t reg_index)
{
	*((volatile uint32_t*)kIOAPICRegIndexAddr) = reg_index;
	return *((volatile uint32_t*)kIOAPICRegDataAddr);
}

static void write_io_apic_register_(LocalAPIC *a, uint8_t reg_index, uint32_t value)
{
	*((volatile uint32_t*)kIOAPICRegIndexAddr) = reg_index;
	*((volatile uint32_t*)kIOAPICRegDataAddr) = value;
}

static uint64_t read_io_apic_redirect_table_register_(LocalAPIC *a, uint8_t irq_index)
{
	return (uint64_t)read_io_apic_register_(a, 0x10 + irq_index * 2) |
		((uint64_t)read_io_apic_register_(a, 0x10 + irq_index * 2 + 1) << 32);
}

static void write_io_apic_redirect_table_register_(LocalAPIC *a, uint8_t irq_index, uint64_t value)
{
	write_io_apic_register_(a, 0x10 + irq_index * 2, (uint32_t)value);
	write_io_apic_register_(a, 0x10 + irq_index * 2 + 1, (uint32_t)(value >> 32));
}

static void set_interrupt_redirection_(LocalAPIC *a,
									  uint64_t local_apic_id,
									  int from_irq_num,
									  int to_vector_index)
{
	uint64_t redirect_table = read_io_apic_redirect_table_register_(a, from_irq_num);
	redirect_table &= 0x00fffffffffe0000UL;
	redirect_table |= (local_apic_id << 56) | to_vector_index;
	write_io_apic_redirect_table_register_(a, from_irq_num, redirect_table);
}

void apic_init(uint64_t local_apic_id)
{
	set_interrupt_redirection_(&g_apic, local_apic_id, 2, 0x20);  // HPET
	set_interrupt_redirection_(&g_apic, local_apic_id, 1, 0x21);  // KBC
}
