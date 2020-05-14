#include "execution_context.h"

#include "gdt.h"

ExecutionContext *execution_context_new(void (*rip)(), void *rsp, uint64_t cr3, uint64_t rflags, uint64_t kernel_rsp) {
	ExecutionContext *ctx = kalloc(ExecutionContext);
	CPUContext *cc = &ctx->cpu_context;
	cc->int_ctx.rip = (uint64_t)rip;
	cc->int_ctx.cs = kUserCS64Selector;
	cc->int_ctx.rsp = (uint64_t)rsp;
	cc->int_ctx.ss = kUserDSSelector;
	cc->int_ctx.rflags = rflags | 2;
	cc->cr3 = cr3;
	ctx->kernel_rsp = kernel_rsp;
	ctx->heap_used_size = 0;
	return ctx;
}
