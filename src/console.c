#include "console.h"

#include <string.h>
#include <stdarg.h>

#include "serial.h"

static const int CONSOLE_BUF_SIZE = 1024;

static Serial *com_;

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

void console_printf(const char *fmt, ...)
{
	char buf[CONSOLE_BUF_SIZE];
	va_list vargs;
	va_start(vargs, fmt);
	vsnprintf(buf, sizeof(buf), fmt, vargs);
	console_write(buf);
	va_end(vargs);
}

void console_printfn(const char *fmt, ...)
{
	char buf[CONSOLE_BUF_SIZE];
	va_list vargs;
	va_start(vargs, fmt);
	vsnprintf(buf, sizeof(buf), fmt, vargs);
	console_write(buf);
	console_write("\n");
	va_end(vargs);
}

