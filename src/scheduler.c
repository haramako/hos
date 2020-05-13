#include "scheduler.h"

#include "asm.h"
#include "timer.h"

Scheduler g_scheduler;

/// Create root process from current execution.
static Process *create_root_process_() {
	ExecutionContext *ctx = execution_context_new(NULL, 0, ReadCR3(), 0, 0);
	Process *p = process_new(ctx);
	return p;
}

void scheduler_init() {
	Scheduler *s = &g_scheduler;
	Process *root_process = create_root_process_();
	scheduler_register_process(root_process);
	s->current = root_process;
	root_process->status = kRunning;
}

void scheduler_register_process(Process *proc) {
	Scheduler *s = &g_scheduler;
	assert(s->number_of_process < kNumberOfProcess);
	assert(proc->status == kNotScheduled);
	s->process[s->number_of_process] = proc;
	proc->scheduler_index = s->number_of_process;
	s->number_of_process++;

	proc->status = kSleeping;
}

#if 0
uint64_t scheduler_launch_and_wait_until_exit(Process *proc) {
	Scheduler *s = &g_scheduler;
	uint64_t t0 = time_now();
	scheduler_register_process(proc);
	process_wait_until_exit(proc);
	uint64_t t1 = time_now();
	return t1 - t0;
}
#endif

Process *scheduler_switch_process() {
	Scheduler *s = &g_scheduler;
	int base_index = s->current->scheduler_index;
	for (int i = 1; i < s->number_of_process; i++) {
		Process *proc = s->process[(base_index + i) % s->number_of_process];
		if (!proc) continue;
		if (proc->status == kSleeping) {
			if (s->current->status == kRunning) s->current->status = kSleeping;
			proc->status = kRunning;
			s->current = proc;
			return proc;
		}
	}
	return NULL;
}

#if 0
void scheduler_kill_current_process() {
	Scheduler *s = &g_scheduler;
	s->current->status = kKilled;
}
#endif

Process *scheduler_current_process() { return g_scheduler.current; }
