#pragma once

#include <stdint.h>
#include <assert.h>

#define UINTN uintptr_t
static_assert(sizeof(UINTN) == 8, "Invalid size.");

#define kBufferSize 0x8000

typedef struct EFI_MemoryMap_ {
	uintptr_t key;
	uintptr_t bytes_used;
	uintptr_t descriptor_size;
	uint32_t descriptor_version;
	uint8_t buf[kBufferSize];
} EFI_MemoryMap;

typedef uint32_t MemoryType;

enum {
	kReserved,
	kLoaderCode,
	kLoaderData,
	kBootServicesCode,
	kBootServicesData,
	kRuntimeServicesCode,
	kRuntimeServicesData,
	kConventionalMemory,
	kUnusableMemory,
	kACPIReclaimMemory,
	kACPIMemoryNVS,
	kMemoryMappedIO,
	kMemoryMappedIOPortSpace,
	kPalCode,
	kPersistentMemory,
	kMaxMemoryType,
};

typedef struct EFI_MemoryDescriptor_ {
	MemoryType type;
	UINTN physical_start;
	UINTN virtual_start;
	UINTN number_of_pages;
	UINTN attribute;
} EFI_MemoryDescriptor;
static_assert(sizeof(EFI_MemoryDescriptor) == 40, "Invalid size.");

int efi_memory_map_get_count(EFI_MemoryMap *mm);
EFI_MemoryDescriptor *efi_memory_map_get_descriptor(EFI_MemoryMap *mm, int idx);
void efi_memory_map_print(EFI_MemoryMap *mm);

void efi_memory_descriptor_print(EFI_MemoryDescriptor *md, int index);
