#pragma once

#include "common.h"

void smp_init();
void smp_entry();

extern uint8_t _trampoline[];
extern uint8_t _trampoline_end[];
