#pragma once

#include "common.h"

#include "asm.h"

typedef struct ExecutionContext {
	CPUContext cpu_context;
	// ProcessMappingInfo map_info;
	uint64_t kernel_rsp;
	uint64_t heap_used_size;
} ExecutionContext;

#if 0
 public:
  uint64_t GetHeapEndVirtAddr() {
    return heap_used_size_ + map_info_.heap.GetVirtAddr();
  }
  void SetCR3(IA_PML4& cr3) {
    cpu_context_.cr3 = reinterpret_cast<uint64_t>(&cr3);
  }
  IA_PML4& GetCR3() { return *reinterpret_cast<IA_PML4*>(cpu_context_.cr3); }
  void Flush(IA_PML4& pml4, uint64_t& stat);
  void CopyContextFrom(ExecutionContext& from, uint64_t& stat_copied_bytes) {
    uint64_t cr3 = cpu_context_.cr3;
    cpu_context_ = from.cpu_context_;
    cpu_context_.cr3 = cr3;

    map_info_.data.CopyDataFrom(from.map_info_.data, stat_copied_bytes);
    map_info_.stack.CopyDataFrom(from.map_info_.stack, stat_copied_bytes);
  }
#endif

ExecutionContext *execution_context_new(void (*rip)(), void *rsp, uint64_t cr3, uint64_t rflags, uint64_t kernel_rsp);
