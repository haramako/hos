#include "console.h"

#include <string.h>
#include <stdarg.h>

#include "asm.h"
#include "serial.h"

static const int CONSOLE_BUF_SIZE = 1024;

static Serial *com_;

int g_console_log_level_ = CONSOLE_LOG_LEVEL_INFO;

const char *LOG_LEVEL_NAMES[] = {
	"T: ",
	"I: ",
	"W: ",
	"E: ",
	"F: ",
};

void console_init(Serial *console_serial)
{
	com_ = console_serial;
}

void console_write(const char *msg)
{
	for(;*msg != '\0'; msg++){
		serial_send_char(com_, *msg);
	}
}

void console_vprintf(const char *fmt, va_list vargs)
{
	char buf[CONSOLE_BUF_SIZE];
	vsnprintf(buf, sizeof(buf), fmt, vargs);
	console_write(buf);
}

void console_printf(const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	console_vprintf(fmt, vargs);
	va_end(vargs);
}

void console_printfn(const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	console_vprintf(fmt, vargs);
	console_write("\n");
	va_end(vargs);
}

void console_log(int log_level, const char *fmt, ...)
{
	assert(log_level >= CONSOLE_LOG_LEVEL_TRACE && log_level <= CONSOLE_LOG_LEVEL_FATAL);
	if( log_level >= g_console_log_level_ ){
		va_list vargs;
		va_start(vargs, fmt);
		console_write(LOG_LEVEL_NAMES[log_level]);
		console_vprintf(fmt, vargs);
		console_write("\n");
		va_end(vargs);
	}
	if( log_level == CONSOLE_LOG_LEVEL_FATAL ){
		Die();
	}
}

void console_set_log_level(int log_level)
{
	g_console_log_level_ = log_level;
}
