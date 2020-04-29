#pragma once

#include "asm.h"
#include "common.h"

typedef void (*InterruptHandler)(uint64_t intcode, InterruptInfo *info);

void interrupt_init();
void interrupt_set_int_handler(uint64_t intcode, InterruptHandler handler);
