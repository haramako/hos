#pragma once

#include "common.h"

struct Serial;

extern int g_console_log_level_;

void console_init(struct Serial *console_serial);
void console_write(const char *msg);
void console_vprintf(const char *fmt, va_list vargs);
void console_printf(const char *fmt, ...);
void console_printfn(const char *fmt, ...);
void console_log(int log_level, const char *fmt, ...);
void console_set_log_level(int log_level);
