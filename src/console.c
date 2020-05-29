#include "console.h"

#include <stdio.h>

#include "asm.h"
#include "boot_param.h"
#include "serial.h"
#include "sheet.h"
#include "sheet_painter.h"

static const int CONSOLE_BUF_SIZE = 1024;

static Serial *com_;

static Sheet sh__;
static Sheet *sh_;
static int x_;
static int y_;
static int FONT_WIDTH = 8;
static int FONT_HEIGHT = 16;
static int FONT_COLS = 800 / 8;
static int FONT_LINE_LEN = 600 / 16;

int g_console_log_level_ = CONSOLE_LOG_LEVEL_INFO;

const char *LOG_LEVEL_NAMES[] = {
	"T: ", "I: ", "W: ", "E: ", "F: ",
};

void console_init(Serial *console_serial, BootParam_Graphics *g) {
	com_ = console_serial;
	sh_ = &sh__;
	memset(sh_, 0, sizeof(Sheet));
	sh_->buf = g->vram;
	sh_->xsize = g->width;
	sh_->ysize = g->height;
	sh_->pixels_per_scan_line = g->pixels_per_scan_line;
	if (sh_) sheet_draw_rect(sh_, 0, 0, sh_->xsize, sh_->ysize, 0, true);
}

void console_write(const char *msg) {
	for (const char *c = msg; *c != '\0'; c++) {
		serial_send_char(com_, *c);
	}
	if (sh_) {
		for (const char *c = msg; *c != '\0'; c++) {
			if (*c == '\n') {
				x_ = 0;
				++y_;
			} else {
				sheet_draw_character(sh_, *c, x_ * FONT_WIDTH, y_ * FONT_HEIGHT, true);
				++x_;
				if (x_ >= FONT_COLS) {
					++y_;
					x_ = 0;
				}
			}

			// Scroll.
			if (y_ >= FONT_LINE_LEN) {
				sheet_scroll(sh_, FONT_HEIGHT, true);
				--y_;
			}
		}
	}
}

void console_vprintf(const char *fmt, va_list vargs) {
	char buf[CONSOLE_BUF_SIZE];
	vsnprintf(buf, sizeof(buf), fmt, vargs);
	console_write(buf);
}

void console_printf(const char *fmt, ...) {
	va_list vargs;
	va_start(vargs, fmt);
	console_vprintf(fmt, vargs);
	va_end(vargs);
}

void console_printfn(const char *fmt, ...) {
	va_list vargs;
	va_start(vargs, fmt);
	console_vprintf(fmt, vargs);
	console_write("\n");
	va_end(vargs);
}

void console_log(int log_level, const char *fmt, ...) {
	assert(log_level >= CONSOLE_LOG_LEVEL_TRACE && log_level <= CONSOLE_LOG_LEVEL_FATAL);
	if (log_level >= g_console_log_level_) {
		va_list vargs;
		va_start(vargs, fmt);
		console_write(LOG_LEVEL_NAMES[log_level]);
		console_vprintf(fmt, vargs);
		console_write("\n");
		va_end(vargs);
	}
	if (log_level == CONSOLE_LOG_LEVEL_FATAL) {
		Die();
	}
}

void console_set_log_level(int log_level) { g_console_log_level_ = log_level; }
