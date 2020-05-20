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

#if 0
 public:
  void Init(uint32_t* buf,
            int xsize,
            int ysize,
            int pixels_per_scan_line,
            int x = 0,
            int y = 0) {
    parent_ = nullptr;
    front_ = nullptr;
    buf_ = buf;
    xsize_ = xsize;
    ysize_ = ysize;
    pixels_per_scan_line_ = pixels_per_scan_line;
    x_ = x;
    y_ = y;
  }
  void SetParent(Sheet* parent) { parent_ = parent; }
  void SetFront(Sheet* front) { front_ = front; }
  int GetXSize() { return xsize_; }
  int GetYSize() { return ysize_; }
  int GetPixelsPerScanLine() { return pixels_per_scan_line_; }
  int GetBufSize() {
    // Assume bytes per pixel == 4
    return ysize_ * pixels_per_scan_line_ * 4;
  }
  uint32_t* GetBuf() { return buf_; }
  void BlockTransfer(int to_x, int to_y, int from_x, int from_y, int w, int h);
  void Flush(int px, int py, int w, int h);

 private:
#endif
