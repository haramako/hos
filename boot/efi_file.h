#pragma once

#include "common.h"

#define kFileNameSize 255

typedef struct {
	#if 0
 public:
  const uint8_t* GetBuf() { return buf_pages_; }
  uint64_t GetFileSize() { return file_size_; }

 private:
  static constexpr int kFileNameSize = 16;
	#endif
  char file_name_[kFileNameSize + 1];
  uint64_t file_size_;
  uint8_t* buf_pages_;
} EFI_File;

//static void Load(EFIFile& dst, const wchar_t* file_name);
