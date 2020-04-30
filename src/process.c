#include "process.h"

void process_new(Process *p, ExecutionContext *ctx) {
	bzero(p, sizeof(*p));
	assert(p->status == kNotInitialized);
	assert(!p->ctx);
	p->ctx = ctx;
	p->status = kNotScheduled;
}

void process_wait_until_exit(Process *p) {
	while (p->status != kKilled) {
		// Sleep();
	}
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
