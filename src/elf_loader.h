#pragma once

#include "common.h"

typedef struct {
	void *code;
	void *data;
	size_t code_size;
	size_t data_size;
	void *entry_point;
} ELFImage;

void elf_load_exec(uint8_t *buf, ELFImage *out_image);
