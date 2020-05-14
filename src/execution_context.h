#pragma once

#include "common.h"

#include "asm.h"

typedef struct ExecutionContext_ {
	CPUContext cpu_context;
	// ProcessMappingInfo map_info;
	uint64_t kernel_rsp;
	uint64_t heap_used_size;
} ExecutionContext;

ExecutionContext *execution_context_new(void (*rip)(), void *rsp, uint64_t cr3, uint64_t rflags, uint64_t kernel_rsp);
