#pragma once

#include "asm.h"
#include "common.h"

void interrupt_init();

typedef void (*InterruptHandler)(uint64_t intcode, InterruptInfo *info);
