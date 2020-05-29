#pragma once

#include <stdint.h>
#include <stdlib.h>

#define kBufferSize 0x8000

typedef struct EFI_MemoryMap_ {
	uintptr_t key;
	uintptr_t bytes_used;
	uintptr_t descriptor_size;
	uint32_t descriptor_version;
	uint8_t buf[kBufferSize];
} EFI_MemoryMap;

typedef struct BootParam_Graphics_ {
	uint32_t *vram;
	uint32_t width;
	uint32_t height;
	uint32_t pixels_per_scan_line;
} BootParam_Graphics;

typedef struct {
	struct ACPI_RSDT_ *rsdt;
	EFI_MemoryMap *efi_memory_map;
	BootParam_Graphics graphics;
	uint8_t *bootfs_buf;
	size_t bootfs_size;
	struct EFI_RuntimeServices_ *efi_runtime_services;
} BootParam;
