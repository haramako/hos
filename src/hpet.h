#pragma once

#include "common.h"

typedef enum HPET_TimerConfig
{
    kUseLevelTriggeredInterrupt = 1 << 1,
    kEnable = 1 << 2,
    kUsePeriodicMode = 1 << 3,
    kSetComparatorValue = 1 << 6,
} HPET_TimerConfig;

typedef struct PACKED TimerRegister {
	HPET_TimerConfig configuration_and_capability;
	uint64_t comparator_value;
	uint64_t fsb_interrupt_route;
	uint64_t reserved;
} TimerRegister;

typedef struct PACKED HPET_RegisterSpace {
	uint64_t general_capabilities_and_id;
	uint64_t reserved00;
	uint64_t general_configuration;
	uint64_t reserved01;
	uint64_t general_interrupt_status;
	uint64_t reserved02;
	uint64_t reserved03[24];
	uint64_t main_counter_value;
	uint64_t reserved04;
	TimerRegister timers[32];
} HPET_RegisterSpace;

typedef struct HPET {
	HPET_TimerConfig configuration_and_capability;
	HPET_RegisterSpace* registers;
	uint64_t femtosecond_per_count;
} HPET;

extern HPET g_hpet;

void hpet_init(HPET_RegisterSpace* registers);
void hpet_set_timer_ms(int timer_index, uint64_t milliseconds, HPET_TimerConfig flags);
void hpet_set_timer_ns(int timer_index, uint64_t nanoseconds, HPET_TimerConfig flags);
uint64_t hpet_read_main_counter_value();
uint64_t hpet_get_femtosecond_per_count();
void hpet_busy_wait(uint64_t ms);
void hpet_print(void);
