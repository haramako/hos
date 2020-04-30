#include "scheduler.h"

#include "asm.h"
#include "timer.h"

Scheduler g_scheduler;

/// Create root procecc from current execution.
static Process *create_root_process_() {
	ExecutionContext *ctx = malloc(sizeof(ExecutionContext));
	uint64_t cr3 = ReadCR3();
	execution_context_new(ctx, NULL, 0, cr3, 0, 0);

	Process *p = malloc(sizeof(Process));
	process_new(p, ctx);

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

uint64_t scheduler_launch_and_wait_until_exit(Process *proc) {
	Scheduler *s = &g_scheduler;
	uint64_t t0 = time_now();
	scheduler_register_process(proc);
	process_wait_until_exit(proc);
	uint64_t t1 = time_now();
	return t1 - t0;
}

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

void scheduler_kill_current_process() {
	Scheduler *s = &g_scheduler;
	s->current->status = kKilled;
}
