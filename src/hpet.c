#include "hpet.h"

#include "asm.h"
#include "console.h"

//namespace GeneralConfigBits {
const uint64_t HPET_GC_ENABLE = 1 << 0;
const uint64_t HPET_USE_LEGACY_REPLACEMENT_ROUTING = 1 << 1;

const uint64_t kMainCounterSupports64bit = 1 << 13;

static uint8_t get_num_of_timers_(uint64_t cap)
{
	return (cap >> 8) & 0b11111;
}

HPET g_hpet;

void hpet_init(HPET_RegisterSpace* registers)
{
	g_hpet.registers = registers;
	g_hpet.femtosecond_per_count = registers->general_capabilities_and_id >> 32;
	uint64_t general_config = registers->general_configuration;
	general_config |= HPET_USE_LEGACY_REPLACEMENT_ROUTING;
	general_config |= HPET_GC_ENABLE;
	g_hpet.registers->general_configuration = general_config;
}

void hpet_set_timer_ms(int timer_index, uint64_t milliseconds, HPET_TimerConfig flags)
{
	hpet_set_timer_ns(timer_index, 1e3 * milliseconds, flags);
}

void hpet_set_timer_ns(int timer_index, uint64_t nanoseconds, HPET_TimerConfig flags)
{
	uint64_t count = 1e9 * nanoseconds / g_hpet.femtosecond_per_count;
	TimerRegister* entry = &g_hpet.registers->timers[timer_index];
	HPET_TimerConfig config = entry->configuration_and_capability;
	HPET_TimerConfig mask = HPET_TC_USE_LEVEL_TRIGGERED_INTERRUPT | HPET_TC_ENABLE | HPET_TC_USE_PERIODIC_MODE;
	config &= ~mask;
	config |= mask & flags;
	config |= HPET_TC_SET_COMPARATOR_VALUE;
	entry->configuration_and_capability = config;
	entry->comparator_value = count;
	g_hpet.registers->main_counter_value = 0;
}

void *mem_physical_to_virtual(void *phy_addr)
{
	return phy_addr;
}

uint64_t hpet_read_main_counter_value()
{
	return ((HPET_RegisterSpace*)mem_physical_to_virtual(g_hpet.registers))->main_counter_value;
}

uint64_t hpet_get_femtosecond_per_count()
{
	return g_hpet.femtosecond_per_count;
}

void hpet_busy_wait(uint64_t ms)
{
	uint64_t count = 1000000000000ULL * ms / g_hpet.femtosecond_per_count + hpet_read_main_counter_value();
	while (hpet_read_main_counter_value() < count) Sleep();
}

void hpet_print(void)
{
	klog("HPET at %016llx", g_hpet.registers);
	klog("  # of timers %d", get_num_of_timers_(g_hpet.registers->general_capabilities_and_id));
	klog("  femtosecond_per_count", g_hpet.femtosecond_per_count);
	klog("  main counter supports 64bit mode %d",
		 g_hpet.registers->general_capabilities_and_id & kMainCounterSupports64bit);
}
