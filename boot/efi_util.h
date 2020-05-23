#pragma once

#include "common.h"

#include "efi.h"

extern EFI efi_;
extern EFI_Handle g_image_handle;
extern EFI_SystemTable *sys_;
extern EFI_MemoryMap g_efi_memory_map;

typedef struct {
	uint64_t file_size;
	uint8_t *buf_pages;
} EFI_File;

void check_status(Status status, const char *msg);

void *efi_allocate_pages(size_t pages);
void *efi_allocate_pages_addr(uintptr_t addr, size_t pages);
void efi_file_load(EFI_File *f, FileProtocol *dir, const char *file_name);
FileProtocol *efi_file_root();
