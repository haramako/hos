#include "sheet.h"

#include "asm.h"

static bool is_in_rect_y_(Sheet *sh, int y) { return 0 <= y && y < sh->ysize; }
static bool is_in_rect_on_parent_(Sheet *sh, int x, int y) {
	return sh->y <= y && y < sh->y + sh->ysize && sh->x <= x && x < sh->x + sh->xsize;
}

void sheet_block_transfer(Sheet *sh, int to_x, int to_y, int from_x, int from_y, int w, int h) {
	uint32_t *b32 = (uint32_t *)(sh->buf);
	if (w & 1) {
		for (int dy = 0; dy < h; dy++) {
			RepeatMove4Bytes(w, &b32[(to_y + dy) * sh->pixels_per_scan_line + (to_x)],
							 &b32[(from_y + dy) * sh->pixels_per_scan_line + (from_x + 0)]);
		}
	} else {
		for (int dy = 0; dy < h; dy++) {
			RepeatMove8Bytes(w / 2, &b32[(to_y + dy) * sh->pixels_per_scan_line + (to_x)],
							 &b32[(from_y + dy) * sh->pixels_per_scan_line + (from_x + 0)]);
		}
	}
	sheet_flush(sh, to_x, to_y, w, h);
}

void sheet_flush(Sheet *sh, int fx, int fy, int w, int h) {
	// Transfer (px, py)(w * h) area to parent
	if (!sh->parent) return;
	assert(0 <= fx && 0 <= fy && (fx + w) <= sh->xsize && (fy + h) <= sh->ysize);
	const int px = fx + sh->x;
	const int py = fy + sh->y;
	if (px >= sh->parent->xsize || py >= sh->parent->ysize || (px + w) < 0 || (py + h) < 0) {
		// This sheet has no intersections with its parent.
		return;
	}
	for (int y = py; y < py + h; y++) {
		if (!is_in_rect_y_(sh->parent, y)) continue;
		const int begin = (0 < px) ? px : 0;
		const int end = (px + w < sh->parent->xsize) ? (px + w) : sh->parent->xsize;
		const int tw = end - begin;
		assert(tw > 0);
		int tbegin = begin;
		for (int x = begin; x < end; x++) {
			bool is_overlapped = false;
			for (Sheet *s = sh->front; s && !is_overlapped; s = s->front) {
				is_overlapped = is_in_rect_on_parent_(s, x, y);
			}
			if (is_overlapped) {
				int tend = x;
				if (tend - tbegin) {
					RepeatMove4Bytes(tend - tbegin, &sh->parent->buf[y * sh->parent->pixels_per_scan_line + tbegin],
									 &sh->buf[(y - sh->y) * sh->pixels_per_scan_line + (tbegin - sh->x)]);
				}
				tbegin = x + 1;
				continue;
			}
		}
		int tend = end;
		if (tend - tbegin) {
			if ((tend - tbegin) & 1) {
				RepeatMove4Bytes(tend - tbegin, &sh->parent->buf[y * sh->parent->pixels_per_scan_line + tbegin],
								 &sh->buf[(y - sh->y) * sh->pixels_per_scan_line + (tbegin - sh->x)]);
			} else {
				RepeatMove8Bytes((tend - tbegin) >> 1, &sh->parent->buf[y * sh->parent->pixels_per_scan_line + tbegin],
								 &sh->buf[(y - sh->y) * sh->pixels_per_scan_line + (tbegin - sh->x)]);
			}
		}
	}
}
