#pragma once

#include "common.h"

#include "boot_param.h"
#include "efi.h"

extern Handle g_image_handle;
extern SystemTable *sys_;

typedef struct {
	uint64_t file_size;
	uint8_t *buf_pages;
} EFI_File;

void check_status(Status status, const char *msg);

void *efi_locate_protocol(GUID *guid);
void *efi_allocate_pages(size_t pages);
void *efi_allocate_pages_addr(uintptr_t addr, size_t pages);
void efi_file_load(EFI_File *f, FileProtocol *dir, const char *file_name);
FileProtocol *efi_file_root();
