#pragma once

#include "common.h"

#include "boot_param.h"
#include "efi_util.h"

typedef struct {
	void *code;
	void *data;
	size_t code_size;
	size_t data_size;
	void *entry_point;
} ELFImage;

void elf_load_kernel(EFI_File *file, ELFImage *out_image);
