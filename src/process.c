#include "process.h"

#include "apic.h"
#include "scheduler.h"
#include "timer.h"

Process *process_new(ExecutionContext *ctx) {
	Process *p = malloc(sizeof(Process));
	bzero(p, sizeof(*p));
	assert(p->status == kNotInitialized);
	assert(!p->ctx);
	p->ctx = ctx;
	p->status = kNotScheduled;
	return p;
}

Process *process_create(void (*entry)()) {
	char *sp = malloc(1024 * 4);
	char *kernel_sp = malloc(1024 * 4);
	uint64_t cr3 = ReadCR3();
	ExecutionContext *ctx =
		execution_context_new(entry, sp + 1024 * 4, cr3, kRFlagsInterruptEnable, (uint64_t)(kernel_sp + 1024 * 4));

	Process *p = process_new(ctx);

	scheduler_register_process(p);

	return p;
}

void process_notify_contextsaving(Process *p) { p->number_of_ctx_switch++; }

void process_print(Process *p) {
	klog("Process:");
	klog(" ID: %d", p->id);
	klog(" Regs:");
	klog("   rip: %016p", p->ctx->cpu_context.int_ctx.rip);
	klog("   rsp: %016p", p->ctx->cpu_context.int_ctx.rsp);
#if 0
	PutString("num of ctx sw, proc time[s], sys time [s], time for ctx save [s], copy "
			  "in ctx save [MB], clflush in ctx sw [M]\n");
	PutDecimal64(number_of_ctx_switch_);
	PutString(", ");
	PutDecimal64WithPointPos(proc_time_femto_sec_, 15);
	PutString(", ");
	PutDecimal64WithPointPos(sys_time_femto_sec_, 15);
	PutString(", ");
	PutDecimal64WithPointPos(time_consumed_in_ctx_save_femto_sec_, 15);
	PutString(", ");
	PutDecimal64WithPointPos(copied_bytes_in_ctx_sw_, 6);
	PutString(", ");
	PutDecimal64WithPointPos(num_of_clflush_issued_in_ctx_sw_, 6);
	PutString("\n");
#endif
}

//============================================================
// Context switch
//============================================================

void process_switch_context(InterruptInfo *int_info, Process *from_proc, Process *to_proc) {
#if 0
	static uint64_t proc_last_time_count = 0;
	const uint64_t now_count = time_now();
	if ((proc_last_time_count - now_count) < liumos->time_slice_count)
		return;

	from_proc.AddProcTimeFemtoSec(
								  (liumos->hpet->ReadMainCounterValue() - proc_last_time_count) *
								  liumos->hpet->GetFemtosecondPerCount());
#endif

	// process_print(from_proc);
	// process_print(to_proc);
	CPUContext *from = &from_proc->ctx->cpu_context;
	const uint64_t t0 = time_now();
	from->cr3 = ReadCR3();
	from->greg = int_info->greg;
	from->int_ctx = int_info->int_ctx;
	process_notify_contextsaving(from_proc);
	const uint64_t t1 = time_now();
	from_proc->time_consumed_in_ctx_save_femto_sec += t1 - t0;

	CPUContext *to = &to_proc->ctx->cpu_context;
	int_info->greg = to->greg;
	int_info->int_ctx = to->int_ctx;
	if (from->cr3 != to->cr3) {
		WriteCR3(to->cr3);
		// proc_last_time_count = liumos->hpet->ReadMainCounterValue();
	}
	// klog("3 %p", int_info->int_ctx.rip);
}

void process_timer_handler(uint64_t intcode, InterruptInfo *info) {
	apic_send_end_of_interrupt(&g_apic);
	// klog("process_test_timer_");
	assert(info);
	Process *proc = g_scheduler.current;
	Process *next_proc = scheduler_switch_process();
	if (!next_proc) return; // no need to switching context.
	process_switch_context(info, proc, next_proc);
}

#if 0
__attribute__((ms_abi)) void SleepHandler(uint64_t intcode, InterruptInfo *info) {
	assert(info);
	Process *proc = g_scheduler.current;
	Process *next_proc = scheduler_switch_process();
	if (!next_proc) return; // no need to switching context.
	process_switch_context(info, proc, next_proc);
}
#else
__attribute__((ms_abi)) void SleepHandler(uint64_t intcode, InterruptInfo *info) {}
#endif

#if 0
Process &ProcessController::Create() {
	Process *proc = kernel_heap_allocator_.AllocPages<Process *>(ByteSizeToPageSize(sizeof(Process)),
																 kPageAttrPresent | kPageAttrWritable);
	new (proc) Process(++last_id_);
	return *proc;
}

static void PrepareContextForRestoringPersistentProcess(ExecutionContext &ctx) {
	SetKernelPageEntries(ctx.GetCR3());
	ctx.SetKernelRSP(liumos->kernel_heap_allocator->AllocPages<uint64_t>(kKernelStackPagesForEachProcess,
																		 kPageAttrPresent | kPageAttrWritable) +
					 (kKernelStackPagesForEachProcess << kPageSizeExponent));
}

Process &ProcessController::RestoreFromPersistentProcessInfo(PersistentProcessInfo &pp_info_in_paddr) {
	PersistentProcessInfo &pp_info = *GetKernelVirtAddrForPhysAddr(&pp_info_in_paddr);

	ExecutionContext &valid_ctx = pp_info.GetValidContext();
	ExecutionContext &working_ctx = pp_info.GetWorkingContext();

	uint64_t dummy_stat;
	working_ctx.CopyContextFrom(valid_ctx, dummy_stat);

	PrepareContextForRestoringPersistentProcess(valid_ctx);
	PrepareContextForRestoringPersistentProcess(working_ctx);

	Process &proc = liumos->proc_ctrl->Create();
	proc.InitAsPersistentProcess(pp_info);

	return proc;
}
#endif
