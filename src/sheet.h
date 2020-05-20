#pragma once

#include "common.h"

typedef struct Sheet_ {
	struct Sheet_ *parent, *front;
	uint32_t *buf;
	int xsize, ysize;
	int x, y;
	int pixels_per_scan_line;
} Sheet;

void sheet_block_transfer(Sheet *sh, int to_x, int to_y, int from_x, int from_y, int w, int h);
void sheet_flush(Sheet *sh, int fx, int fy, int w, int h);
