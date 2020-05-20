#pragma once
#include "sheet.h"

void sheet_draw_character(Sheet *sh, char c, int px, int py, bool do_flush);
void sheet_draw_rect(Sheet *sh, int px, int py, int w, int h, uint32_t col, bool do_flush);
void sheet_draw_point(Sheet *sh, int px, int py, uint32_t col, bool do_flush);
void sheet_scroll(Sheet *sh, int y, bool do_flush);
