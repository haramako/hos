#include "sheet_painter.h"

#include "asm.h"

// @font.gen.c
extern uint8_t font[0x100][16];

void sheet_draw_character(Sheet *sh, char c, int px, int py, bool do_flush) {
	if (!sh->buf) return;
	uint32_t *b32 = (uint32_t *)(sh->buf);
	for (int dy = 0; dy < 16; dy++) {
		for (int dx = 0; dx < 8; dx++) {
			uint32_t col = ((font[(uint8_t)c][dy] >> (7 - dx)) & 1) ? 0xffffff : 0;
			int x = px + dx;
			int y = py + dy;
			b32[y * sh->pixels_per_scan_line + x] = col;
		}
	}
	if (do_flush) sheet_flush(sh, px, py, 8, 16);
}

void sheet_draw_rect(Sheet *sh, int px, int py, int w, int h, uint32_t col, bool do_flush) {
	if (!sh->buf) return;
	uint32_t *b32 = (uint32_t *)(sh->buf);
	if (w & 1) {
		for (int y = py; y < py + h; y++) {
			RepeatStore4Bytes(w, &b32[y * sh->pixels_per_scan_line + px], col);
		}
	} else {
		for (int y = py; y < py + h; y++) {
			RepeatStore8Bytes(w / 2, &b32[y * sh->pixels_per_scan_line + px], ((uint64_t)col << 32) | col);
		}
	}
	if (do_flush) sheet_flush(sh, px, py, w, h);
}

void sheet_draw_point(Sheet *sh, int px, int py, uint32_t col, bool do_flush) {
	sh->buf[py * sh->pixels_per_scan_line + px] = col;
	if (do_flush) sheet_flush(sh, px, py, 1, 1);
}
