#pragma once

#include "common.h"

__attribute__((ms_abi)) uint64_t asm_read_cr3(void);
__attribute__((ms_abi)) void asm_write_cr3(uint64_t);
__attribute__((ms_abi)) void asm_jump_to_kernel(void *kernel_entry_point, void *data, uint64_t kernel_stack_pointer);
