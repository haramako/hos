#pragma once

#include "common.h"
#include "efi/guid.h"
#include "efi/memory_map.h"
#include "efi/runtime_services.h"

#define UINTN uintptr_t
static_assert(sizeof(UINTN) == 8, "Invalid size.");

typedef UINTN AllocateType;

typedef void *Handle;

typedef UINTN Status;
#define Status_kSuccess 0x0
#define Status_kBufferTooSmall 0x8000000000000005
#define Status_kNotFound 0x800000000000000E

typedef struct TableHeader_ {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
} TableHeader;
static_assert(sizeof(TableHeader) == 24, "Invalid size.");

typedef struct {
	TableHeader header;
	uint64_t _buf_0[2];
	Status (*AllocatePages)(AllocateType, MemoryType, UINTN pages, void **mem);
	Status (*FreePages)();
	Status (*GetMemoryMap)(UINTN *memory_map_size, uint8_t *, UINTN *map_key, UINTN *descriptor_size,
						   uint32_t *descriptor_version);
	uint64_t _buf5_0[11];
	Status (*HandleProtocol)(Handle, const GUID *, void **);
	uint64_t _buf5_1[9];
	Status (*ExitBootServices)(void *image_handle, UINTN map_key);
	uint64_t _buf7[10];
	uint64_t (*LocateProtocol)(const GUID *Protocol, void *Registration, void **Interface);
	uint64_t _buf10_2[6];
} BootServices;

typedef struct {
	GUID vendor_guid;
	void *vendor_table;
} ConfigurationTable;

typedef struct SimpleTextOutputProtocol {
	uint64_t _buf;
	uint64_t (*output_string)(struct SimpleTextOutputProtocol *, const wchar_t *);
	uint64_t _buf4[8];
} SimpleTextOutputProtocol;

typedef struct SystemTable_ {
	TableHeader header;
	wchar_t *firmware_vendor;
	uint32_t firmware_revision;
	Handle console_in_handle;
	struct SimpleTextInputProtocol *con_in;
	Handle console_out_handle;
	SimpleTextOutputProtocol *con_out;
	Handle standard_error_handle;
	SimpleTextOutputProtocol *std_err;
	EFI_RuntimeServices *runtime_services;
	BootServices *boot_services;
	UINTN number_of_table_entries;
	ConfigurationTable *configuration_table;
} SystemTable;

typedef uint64_t FileProtocolModes;
#define kRead 1ULL

#define kFileNameSize 16

typedef struct {
	uint8_t dummy[16];
} Time;

typedef struct FileInfo {
	uint64_t size;
	uint64_t file_size;
	uint64_t physical_size;
	Time create_time;
	Time last_access_time;
	Time modification_time;
	uint64_t attr;
	wchar_t file_name[kFileNameSize];
} FileInfo;

typedef struct FileProtocol {
	uint64_t revision;
	Status (*Open)(struct FileProtocol *self, struct FileProtocol **new_handle, const wchar_t *rel_path,
				   FileProtocolModes mode, uint64_t attr);
	Status (*Close)();
	Status (*Delete)();
	Status (*Read)(struct FileProtocol *self, UINTN *buffer_size, void *buffer);
	uint64_t (*Write)();
	uint64_t _buf3[2];
	Status (*GetInfo)(struct FileProtocol *self, const GUID *type, UINTN *buffer_size, uint8_t *buffer);
	uint64_t _buf4;
	uint64_t (*Flush)();
} FileProtocol;

typedef struct SimpleFileSystemProtocol {
	uint64_t Revision;
	Status (*OpenVolume)(struct SimpleFileSystemProtocol *self, FileProtocol **Root);
} SimpleFileSystemProtocol;

typedef struct LoadedImageProtocol {
	uint32_t revision;
	Handle parent_handle;
	SystemTable *system_table;
	Handle device_handle;
	struct _DEVICE_PATH_PROTOCOL *file_path;
	void *reserved;
	uint32_t load_options_size;
	void *load_options;
	void *image_base;
	uint64_t image_size;
	MemoryType image_code_type;
	MemoryType image_data_type;
	Status (*unload)(Handle ImageHandle);
} LoadedImageProtocol;

enum {
	kAnyPages,
	kMaxAddress,
	kAddress,
};

typedef struct {
	uint64_t _buf[3];
	struct ModeInfo {
		uint32_t max_mode;
		uint32_t mode;
		struct {
			uint32_t version;
			uint32_t horizontal_resolution;
			uint32_t vertical_resolution;
			uint32_t pixel_format;
			struct {
				uint32_t red_mask;
				uint32_t green_mask;
				uint32_t blue_mask;
				uint32_t reserved_mask;
			} pixel_info;
			uint32_t pixels_per_scan_line;
		} * info;
		UINTN size_of_info;
		void *frame_buffer_base;
		UINTN frame_buffer_size;
	} * mode;
} GraphicsOutputProtocol;
