#pragma once

#include "common.h"

#include "boot_param.h"
#include "efi.h"

extern Handle g_image_handle;
extern SystemTable *g_sys;

typedef struct {
	uint64_t file_size;
	uint8_t *buf;
} EFI_File;

void efi_check_status(Status status, const char *msg);

void *efi_locate_protocol(GUID *guid);
void *efi_allocate_pages(size_t pages);
void *efi_allocate_pages_addr(uintptr_t addr, size_t pages);

FileProtocol *efi_file_root();
void efi_file_load(EFI_File *out_file, FileProtocol *dir, const char *file_name);
