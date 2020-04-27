#include "hpet.h"

#include "asm.h"

// HPET General Configuration
#define HPET_GC_ENABLE (1ULL << 0)
#define HPET_GC_USE_LEGACY_REPLACEMENT_ROUTING (1ULL << 1)

#define kMainCounterSupports64bit (1ULL << 13)

static uint8_t get_num_of_timers_(uint64_t cap) { return (cap >> 8) & 0b11111; }

HPET g_hpet;

void hpet_init(HPET_RegisterSpace *registers) {
	g_hpet.registers = registers;
	g_hpet.femtosecond_per_count = registers->general_capabilities_and_id >> 32;
	uint64_t general_config = registers->general_configuration;
	general_config |= HPET_GC_USE_LEGACY_REPLACEMENT_ROUTING;
	general_config |= HPET_GC_ENABLE;
	g_hpet.registers->general_configuration = general_config;

	// Disable timers.
	hpet_set_timer_ns(0, 0, 0);
	hpet_set_timer_ns(1, 0, 0);
}

void hpet_set_timer_ms(int timer_index, uint64_t milliseconds, HPET_TimerConfig flags) {
	hpet_set_timer_ns(timer_index, 1e3 * milliseconds, flags);
}

void hpet_enable(bool enable) {
	uint64_t general_config = g_hpet.registers->general_configuration;
	if (enable) {
		general_config |= HPET_GC_ENABLE;
	} else {
		general_config &= ~HPET_GC_ENABLE;
	}
	g_hpet.registers->general_configuration = general_config;
}

void hpet_set_timer_ns(int timer_index, uint64_t nanoseconds, HPET_TimerConfig flags) {
	uint64_t count = 1e9 * nanoseconds / g_hpet.femtosecond_per_count;
	TimerRegister *entry = &g_hpet.registers->timers[timer_index];
	HPET_TimerConfig config = entry->configuration_and_capability;
	HPET_TimerConfig mask = HPET_TC_USE_LEVEL_TRIGGERED_INTERRUPT | HPET_TC_ENABLE | HPET_TC_USE_PERIODIC_MODE;
	config &= ~mask;
	config |= mask & flags;
	config |= HPET_TC_SET_COMPARATOR_VALUE;
	entry->configuration_and_capability = config;
	entry->comparator_value = hpet_read_main_counter_value() + count;
	entry->comparator_value = count;

	ktrace("Set HPET timer flags=%0x16llx, count: %lld", flags, count);
}

void *mem_physical_to_virtual(void *phy_addr) { return phy_addr; }

uint64_t hpet_read_main_counter_value() {
	return ((HPET_RegisterSpace *)mem_physical_to_virtual(g_hpet.registers))->main_counter_value;
}

uint64_t hpet_get_femtosecond_per_count() { return g_hpet.femtosecond_per_count; }

void hpet_busy_wait(uint64_t ms) {
	uint64_t count = 1000000000000ULL * ms / g_hpet.femtosecond_per_count + hpet_read_main_counter_value();
	while (hpet_read_main_counter_value() < count)
		; // Sleep();
}

void hpet_print(void) {
	int num_timers = get_num_of_timers_(g_hpet.registers->general_capabilities_and_id);

	uint64_t gc = g_hpet.registers->general_configuration;
	klog("HPET info:");
	klog("  Addr: %016llx", g_hpet.registers);
	klog("  Num timers: %d", num_timers);
	klog("  Femto second per count: %lld", g_hpet.femtosecond_per_count);
	klog("  Supports 64bit mode: %d", !!(g_hpet.registers->general_capabilities_and_id & kMainCounterSupports64bit));
	klog("  Enabled: %d", gc & 1);
	klog("  Legacy replacement: %d", !!(gc & (1 << 1)));
	klog("  Time: %lld", hpet_read_main_counter_value());
	klog("  Time Sec: %f", hpet_read_main_counter_value() / (1.0 / g_hpet.femtosecond_per_count * 1e15));

	klog("  Timers:");
	for (int i = 0; i < num_timers; i++) {
		TimerRegister *entry = &g_hpet.registers->timers[i];
		HPET_TimerConfig config = entry->configuration_and_capability;
		klog("    %d: config=%016llx, comparator_value=%lld", i, config, entry->comparator_value);
	}
}
