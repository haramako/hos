#pragma once

#include "common.h"
#include "asm.h"

void interrupt_init();

typedef void (*InterruptHandler)(uint64_t intcode, InterruptInfo* info);
