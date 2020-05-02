#pragma once

#include "common.h"

#define UINTN uintptr_t

#define kBufferSize 0x8000

typedef struct EFI_MemoryMap_ {
	uintptr_t key;
	uintptr_t bytes_used;
	uintptr_t descriptor_size;
	uint8_t buf[kBufferSize];
} EFI_MemoryMap;

typedef enum MemoryType_ {
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
} MemoryType;

typedef struct EFI_MemoryDescriptor_ {
	MemoryType type;
	UINTN physical_start;
	UINTN virtual_start;
	UINTN number_of_pages;
	UINTN attribute;
} EFI_MemoryDescriptor;

int efi_memory_map_get_count(EFI_MemoryMap *mm);
EFI_MemoryDescriptor *efi_memory_map_get_descriptor(EFI_MemoryMap *mm, int idx);
void efi_memory_map_print(EFI_MemoryMap *mm);

void efi_memory_descriptor_print(EFI_MemoryDescriptor *md, int index);

// Dummy decralations
typedef void *Handle;
typedef void GraphicsOutputProtocol;
typedef void SimpleFileSystemProtocol;
typedef void FileProtocol;
typedef void *TableHeader;
typedef void SimpleTextInputProtocol;
typedef void SimpleTextOutputProtocol;
typedef void BootServices;
typedef void ConfigurationTable;

typedef enum { EfiResetCold, EfiResetWarm, EfiResetShutdown, EfiResetPlatformSpecific } ResetType;

typedef struct EFI_RuntimeServices_ {
	char _buf_rs1[24];
	uint64_t _buf_rs2[4];

	int (*set_virtual_address_map)(UINTN memory_map_size, UINTN descriptor_size, UINTN descriptor_version,
								   void *virtual_map);
	void (*convert_pointer)(); // dummy
	uint64_t _buf_rs4[3];
	uint64_t _buf_rs5;
	void (*reset_system)(ResetType, uint64_t reset_status, uint64_t data_size, void *);
} EFI_RuntimeServices;

typedef struct EFI_TableHeader_ {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
} EFI_TableHeader;
// static_assert(sizeof(EFI_TableHeader) == 24);

typedef struct EFI_SystemTable_ {
	EFI_TableHeader header;
	wchar_t *firmware_vendor;
	uint32_t firmware_revision;
	Handle console_in_handle;
	SimpleTextInputProtocol *con_in;
	Handle console_out_handle;
	SimpleTextOutputProtocol *con_out;
	Handle standard_error_handle;
	SimpleTextOutputProtocol *std_err;
	EFI_RuntimeServices *runtime_services;
	BootServices *boot_services;
	UINTN number_of_table_entries;
	ConfigurationTable *configuration_table;
} EFI_SystemTable;

typedef struct EFI_ {
	Handle image_handle;
	EFI_SystemTable *system_table;
	GraphicsOutputProtocol *graphics_output_protocol;
	SimpleFileSystemProtocol *simple_fs;
	FileProtocol *root_file;
} EFI;
