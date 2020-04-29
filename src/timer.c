#include "timer.h"

#include <strings.h>

#include "apic.h"
#include "asm.h"
#include "hpet.h"
#include "interrupt.h"

TimerList g_timer;

static void refresh_timers_();

uint64_t time_now() { return 1e6 * hpet_read_main_counter_value() / g_hpet.femtosecond_per_count; }

static void handler_(uint64_t intcode, InterruptInfo *info) {
	apic_send_end_of_interrupt(&g_apic);

	TimerParam param;
	param.time = time_now();

	for (int i = 0; i < TIMER_LEN; i++) {
		Timer *t = &g_timer.timers[i];
		if (t->type == TIMER_TYPE_NONE) continue;

		switch (t->type) {
		case TIMER_TYPE_ONESHOT:
			t->callback(&param, t->data);
			t->type = TIMER_TYPE_NONE;
			break;
		case TIMER_TYPE_PERIODIC:
			t->callback(&param, t->data);
			t->time += t->interval;
			break;
		default:
			kpanic("Invalid timer type");
			break;
		}
	}

	refresh_timers_();
}

void timer_init() {
	bzero(&g_timer, sizeof(g_timer));
	interrupt_set_int_handler(0x20, handler_);
}

Timer *new_timer_() {
	for (int i = 0; i < TIMER_LEN; i++) {
		if (g_timer.timers[i].type == TIMER_TYPE_NONE) {
			return &g_timer.timers[i];
		}
	}
	return NULL;
}

static void refresh_timers_() {
	// Calculate next interrupt time.
	uint64_t now = time_now();
	uint64_t min_time = UINT64_MAX;
	for (int i = 0; i < TIMER_LEN; i++) {
		Timer *t = &g_timer.timers[i];
		if (t->type == TIMER_TYPE_NONE) continue;

		if (t->time < min_time) {
			min_time = t->time;
		}
	}

	klog("min time = %lld", min_time);

	if (min_time == UINT64_MAX) {
		hpet_set_timer_ns(0, 0, 0);
	} else {
		// Set timer hander.
		hpet_set_timer_ns(0, min_time, HPET_TC_ENABLE);
	}
}

Timer *timer_call_at(uint64_t time_ns, TimerCallback callback, void *data) {
	Timer *t = new_timer_();
	kcheck(t, "Can't create timer.");

	t->type = TIMER_TYPE_ONESHOT;
	t->time = time_ns;
	t->callback = callback;
	t->data = data;

	refresh_timers_();

	return t;
}

Timer *timer_call_after(uint64_t time_ns, TimerCallback callback, void *data) {
	return timer_call_at(time_now() + time_ns, callback, data);
}

Timer *timer_call_periodic(uint64_t interval_ns, TimerCallback callback, void *data) {
	Timer *t = new_timer_();
	kcheck(t, "Can't create timer.");

	t->type = TIMER_TYPE_PERIODIC;
	t->time = time_now() + interval_ns;
	t->interval = interval_ns;
	t->callback = callback;
	t->data = data;

	refresh_timers_();

	return t;
}

void timer_free(Timer *timer) {
	kcheck((timer->type > TIMER_TYPE_NONE) && (timer->type < TIMER_TYPE_MAX), "Invalid TimeType");

	timer->type = TIMER_TYPE_NONE;
}

void timer_print() {}
