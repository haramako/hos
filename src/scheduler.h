#pragma once

#include "common.h"

#include "process.h"

#define kNumberOfProcess 256

typedef struct Scheduler {
	Process *process[kNumberOfProcess];
	int number_of_process;
	Process *current;
} Scheduler;

extern Scheduler g_scheduler;

void scheduler_init();
void scheduler_register_process(Process *proc);
Process *scheduler_switch_process();
void scheduler_kill_current_process();
