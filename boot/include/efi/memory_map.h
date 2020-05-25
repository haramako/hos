#pragma once

#include <assert.h>
#include <stdint.h>

#define UINTN uintptr_t
static_assert(sizeof(UINTN) == 8, "Invalid size.");

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
