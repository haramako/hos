#pragma once

#include "common.h"

#include "serial.h"

void console_init(Serial *console_serial);
void console_write(const char *msg);
void console_printf(const char *fmt, ...);
void console_printfn(const char *fmt, ...);

#define klog console_printfn
