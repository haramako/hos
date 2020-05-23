#pragma once

#include "common.h"
#include "guid.h"

#define UINTN uintptr_t

#define kBufferSize 0x8000

typedef UINTN EFI_Status;

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
} MemoryType__;

typedef UINTN AllocateType;

typedef uint32_t MemoryType;

typedef struct EFI_MemoryDescriptor_ {
	MemoryType type;
	UINTN physical_start;
	UINTN virtual_start;
	UINTN number_of_pages;
	UINTN attribute;
} EFI_MemoryDescriptor;

// Dummy decralations
typedef void *EFI_Handle;
typedef void *Handle;
typedef void GraphicsOutputProtocol;
typedef void SimpleTextInputProtocol;

typedef UINTN Status;
#define Status_kSuccess 0x0
#define Status_kBufferTooSmall 0x8000000000000005
#define Status_kNotFound 0x800000000000000E

typedef struct EFI_TableHeader_ {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
} EFI_TableHeader;
static_assert(sizeof(EFI_TableHeader) == 24, "Invalid size.");

typedef struct BootServices {
	EFI_TableHeader header;
	Status (*RaiseTPL)();
	Status (*RestoreTPL)();
	Status (*AllocatePages)(AllocateType, MemoryType, UINTN pages, void **mem);
	Status (*FreePages)();
	Status (*GetMemoryMap)(UINTN *memory_map_size, uint8_t *, UINTN *map_key, UINTN *descriptor_size,
						   uint32_t *descriptor_version);
	uint64_t (*AllocatePool)(MemoryType, uint64_t, void **);
	uint64_t (*FreePool)(void *Buffer);
	uint64_t (*CreateEvent)(unsigned int Type, uint64_t NotifyTpl, void (*NotifyFunction)(void *Event, void *Context),
							void *NotifyContext, void *Event);
	// uint64_t (*SetTimer)(void* Event, TimerDelay, uint64_t TriggerTime);
	void *SetTimer;
	uint64_t (*WaitForEvent)(uint64_t NumberOfEvents, void **Event, uint64_t *Index);
	Status (*SignalEvent)();
	Status (*CloseEvent)();
	Status (*CheckEvent)();
	// Protocol Handler Services
	uint64_t _buf5[3];
	Status (*HandleProtocol)(Handle, const GUID *, void **);
	uint64_t _buf5_1[2];
	Status (*LocateHandle)();
	uint64_t _buf5_2[2];
// Image Services
#if 0
	uint64_t (*LoadImage)(unsigned char BootPolicy,
                          void* ParentImageHandle,
                          DevicePathProtocol*,
                          void* SourceBuffer,
                          uint64_t SourceSize,
                          void** ImageHandle);
#else
	void *LoadImage;
#endif
	uint64_t (*StartImage)(void *ImageHandle, uint64_t *ExitDataSize, unsigned short **ExitData);
	uint64_t _buf6[2];
	Status (*ExitBootServices)(void *image_handle, UINTN map_key);
	uint64_t _buf7[2];
	uint64_t (*SetWatchdogTimer)(uint64_t Timeout, uint64_t WatchdogCode, uint64_t DataSize,
								 unsigned short *WatchdogData);
	uint64_t _buf8[2];
	uint64_t (*OpenProtocol)(void *Handle, GUID *Protocol, void **Interface, void *AgentHandle, void *ControllerHandle,
							 unsigned int Attributes);
	uint64_t _buf9[2];
	uint64_t _buf10[2];
	uint64_t (*LocateProtocol)(const GUID *Protocol, void *Registration, void **Interface);
	uint64_t _buf10_2[2];
	uint64_t _buf11;
	void (*CopyMem)(void *Destination, void *Source, uint64_t Length);
	void (*SetMem)(void *Buffer, uint64_t Size, unsigned char Value);
	uint64_t _buf12;
} BootServices;

typedef struct ConfigurationTable_ {
	GUID vendor_guid;
	void *vendor_table;
} ConfigurationTable;

typedef struct SimpleTextOutputProtocol {
	uint64_t _buf;
	uint64_t (*output_string)(struct SimpleTextOutputProtocol *, const wchar_t *);
	uint64_t (*test_string)(struct SimpleTextOutputProtocol *, wchar_t *);
	uint64_t (*query_mode)(struct SimpleTextOutputProtocol *, wchar_t *, uint64_t *columns, uint64_t *rows);
	uint64_t (*set_mode)(struct SimpleTextOutputProtocol *, uint64_t);
	uint64_t (*set_attribute)(struct SimpleTextOutputProtocol *, uint64_t Attribute);
	uint64_t (*clear_screen)(struct SimpleTextOutputProtocol *);
	uint64_t _buf4[2];
	struct SIMPLE_TEXT_OUTPUT_MODE {
		int MaxMode;
		int Mode;
		int Attribute;
		int CursorColumn;
		int CursorRow;
		uint8_t CursorVisible;
	} * Mode;
} SimpleTextOutputProtocol;

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

typedef struct EFI_SystemTable_ {
	EFI_TableHeader header;
	wchar_t *firmware_vendor;
	uint32_t firmware_revision;
	EFI_Handle console_in_handle;
	SimpleTextInputProtocol *con_in;
	EFI_Handle console_out_handle;
	SimpleTextOutputProtocol *con_out;
	EFI_Handle standard_error_handle;
	SimpleTextOutputProtocol *std_err;
	EFI_RuntimeServices *runtime_services;
	BootServices *boot_services;
	UINTN number_of_table_entries;
	ConfigurationTable *configuration_table;
} EFI_SystemTable;

typedef uint64_t FileProtocolModes;
#define kRead 1ULL

#define kFileNameSize 16

typedef struct Time {
	uint16_t Year;  // 1900 – 9999
	uint8_t Month;  // 1 – 12
	uint8_t Day;	// 1 – 31
	uint8_t Hour;   // 0 – 23
	uint8_t Minute; // 0 – 59
	uint8_t Second; // 0 – 59
	uint8_t Pad1;
	uint32_t Nanosecond; // 0 – 999,999,999
	uint16_t TimeZone;   // -1440 to 1440 or 2047
	uint8_t Daylight;
	uint8_t Pad2;
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
	EFI_SystemTable *system_table;
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

typedef struct EFI_ {
	EFI_Handle image_handle;
	EFI_SystemTable *system_table;
	GraphicsOutputProtocol *graphics_output_protocol;
	SimpleFileSystemProtocol *simple_fs;
	FileProtocol *root_file;
} EFI;
