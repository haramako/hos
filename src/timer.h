#pragma once

#include "common.h"

#define TIMER_LEN 256

typedef struct TimerParam_ {
	uint64_t time;
} TimerParam;

typedef enum TimerType_ {
	TIMER_TYPE_NONE,
	TIMER_TYPE_ONESHOT,
	TIMER_TYPE_PERIODIC,
	TIMER_TYPE_MAX,
} TimerType;

typedef void (*TimerCallback)(TimerParam *timer_param, void *data);

typedef struct Timer {
	TimerType type;
	uint64_t time;
	uint64_t interval;
	TimerCallback callback;
	void *data;
} Timer;

typedef struct TimerList {
	Timer timers[TIMER_LEN];
} TimerList;

extern TimerList g_timer;

uint64_t time_now();

void timer_init();
Timer *timer_call_at(uint64_t time_ns, TimerCallback callback, void *data);
Timer *timer_call_after(uint64_t time_ns, TimerCallback callback, void *data);
Timer *timer_call_periodic(uint64_t interval_ns, TimerCallback callback, void *data);
void timer_free(Timer *timer);
void timer_print();
