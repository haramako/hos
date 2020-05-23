#pragma once

#include "common.h"

#include "efi.h"

typedef struct {
	char file_name[kFileNameSize + 1];
	uint64_t file_size;
	uint8_t *buf_pages;
} EFI_File;

void efi_file_load(EFI_File *f, FileProtocol *dir, const char *file_name);
FileProtocol *efi_file_root();
