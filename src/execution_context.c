#include "execution_context.h"

#if 0
void SegmentMapping::Print() {
	PutString("vaddr:");
	PutHex64ZeroFilled(vaddr_);
	PutString(" paddr:");
	PutHex64ZeroFilled(paddr_);
	PutString(" size:");
	PutHex64(map_size_);
	PutChar('\n');
}

void SegmentMapping::AllocSegmentFromPersistentMemory(PersistentMemoryManager &pmem) {
	SetPhysAddr(pmem.AllocPages<uint64_t>(ByteSizeToPageSize(GetMapSize())));
}

void SegmentMapping::CopyDataFrom(SegmentMapping &from, uint64_t &stat_copied_bytes) {
	assert(map_size_ == from.map_size_);
	memcpy(reinterpret_cast<void *>(GetKernelVirtAddrForPhysAddr(paddr_)),
		   reinterpret_cast<void *>(GetKernelVirtAddrForPhysAddr(from.paddr_)), map_size_);
	stat_copied_bytes += map_size_;
};

void SegmentMapping::Flush(IA_PML4 &pml4, uint64_t &num_of_clflush_issued) {
	FlushDirtyPages(pml4, vaddr_, map_size_, num_of_clflush_issued);
}

void ProcessMappingInfo::Print() {
	PutString("code : ");
	code.Print();
	PutString("data : ");
	data.Print();
	PutString("stack: ");
	stack.Print();
}

#endif

#if 0
void execution_context_expandheap(int64_t diff) {
	uint64_t diff_abs = diff < 0 ? -diff : diff;
	if (diff_abs > map_info_.heap.GetMapSize()) {
		PutStringAndDecimal("diff_abs", diff_abs);
		PutStringAndDecimal("map_size", map_info_.heap.GetMapSize());
		Panic("Too large heap expansion request");
	}
	heap_used_size_ += diff;
	if (heap_used_size_ > map_info_.heap.GetMapSize()) Panic("No more heap");
};

void execution_context_flush(IA_PML4 &pml4, uint64_t &num_of_clflush_issued) {
	map_info_.Flush(pml4, num_of_clflush_issued);
	CLFlush(this, sizeof(*this), num_of_clflush_issued);
}
#endif

void execution_context_new(ExecutionContext *ctx, void (*rip)(), void *rsp, uint64_t cr3, uint64_t rflags,
						   uint64_t kernel_rsp) {
	bzero(ctx, sizeof(ExecutionContext));
	CPUContext *cc = &ctx->cpu_context;
	cc->int_ctx.rip = (uint64_t)rip;
	cc->int_ctx.rsp = (uint64_t)rsp;
	cc->int_ctx.rflags = rflags | 2;
	cc->cr3 = cr3;
	ctx->kernel_rsp = kernel_rsp;
	ctx->heap_used_size = 0;
}
