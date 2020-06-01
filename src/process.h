#pragma once

#include "common.h"

#include "execution_context.h"
#include "mm.h"

typedef enum ProcessStatus_ {
	kNotInitialized,
	kNotScheduled,
	kSleeping,
	kRunning,
	kKilled,
} ProcessStatus;

#define FD_MAX 16

typedef struct {
	struct INode_ *inode;
	size_t pos;
} FileDescriptor;

typedef struct Process_ {
	uint64_t id;
	ProcessStatus status;
	int scheduler_index;
	ExecutionContext *ctx;
	MemoryMap *mm;
	int exit_code;

	int fd_num;
	FileDescriptor fds[FD_MAX];

	uint64_t number_of_ctx_switch;
	uint64_t proc_time_femto_sec;
	uint64_t sys_time_femto_sec;
	uint64_t copied_bytes_in_ctx_sw;
	uint64_t num_of_clflush_issued_in_ctx_sw;
	uint64_t time_consumed_in_ctx_save_femto_sec;
} Process;

typedef struct ProcessCreateParam_ {
	void (*entry_point)();
	int priority;
	size_t stack_size;
	size_t kernel_stack_size;
	union PageMapEntry_ *pml4;
} ProcessCreateParam;

Process *process_new(ExecutionContext *ctx);
Process *process_create(ProcessCreateParam *p);
void process_exit(Process *p, int exit_code);

void process_notify_contextsaving(Process *p);
void process_print(Process *p);

void process_switch_context(InterruptInfo *int_info, Process *from_proc, Process *to_proc);
void process_timer_handler(uint64_t intcode, InterruptInfo *info);
