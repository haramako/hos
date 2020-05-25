#include "efi_util.h"

#include "console.h"
#include "efi.h"
#include "page.h"
#include "util.h"

Handle g_image_handle;
SystemTable *sys_;

static const GUID kFileInfoGUID = {0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

static const GUID kSimpleFileSystemProtocolGUID = {
	0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

static const GUID kFileSystemInfoGUID = {
	// EFI_FILE_SYSTEM_INFO_ID
	0x09576e93,
	0x6d3f,
	0x11d2,
	{0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

static const GUID kLoadedImageProtocolGUID = {
	0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};

void efi_check_status(Status status, const char *msg) {
	if (status != Status_kSuccess) {
		print(msg);
		print_hex(", status = ", status);
		panic("");
	}
}

void *efi_locate_protocol(GUID *guid) {
	void *protocol = NULL;
	Status status = sys_->boot_services->LocateProtocol(guid, NULL, &protocol);
	efi_check_status(status, "Can't locate protocol.");
	return protocol;
}

FileProtocol *efi_open_file(FileProtocol *dir, char *path) {
	wchar_t wpath[kFileNameSize];
	FileProtocol *file = NULL;
	Status status = dir->Open(dir, &file, str2wstr(wpath, path, kFileNameSize), kRead, 0);
	if (status != Status_kSuccess || !file) {
		print("Failed to open file ");
		print(path);
		print("\n");
		panic("");
	}
	return file;
}

void *efi_allocate_pages(size_t pages) {
	void *data;
	Status status = sys_->boot_services->AllocatePages(kAnyPages, kLoaderData, pages, &data);
	efi_check_status(status, "AllocatePages() failed.");
	return data;
}

void *efi_allocate_pages_addr(uintptr_t addr, size_t pages) {
	uintptr_t allocated = (uintptr_t)efi_allocate_pages(pages);
	page_map_addr(allocated, addr, (int)pages);
	return (void *)addr;
}

Status efi_handle_protocol(Handle handle, GUID *guid, void **out) {
	return sys_->boot_services->HandleProtocol(g_image_handle, guid, out);
}

FileProtocol *efi_file_root() {
	Status status;

	LoadedImageProtocol *loaded_image;
	status = efi_handle_protocol(g_image_handle, &kLoadedImageProtocolGUID, (void **)&loaded_image);
	efi_check_status(status, "Can't get LoadedImageProtocol.");
	// print_hex("image ", (uint64_t)loaded_image->image_base);
	// print_hex("image ", (uint64_t)efi_file_root);

	SimpleFileSystemProtocol *simple_fs;
	status = sys_->boot_services->HandleProtocol(loaded_image->device_handle, &kSimpleFileSystemProtocolGUID,
												 (void **)&simple_fs);
	efi_check_status(status, "Can't get SimpleFileSystemProtocol.");

	FileProtocol *root;
	simple_fs->OpenVolume(simple_fs, &root);
	efi_check_status(status, "OpenVolume() failed.");
	return root;
}

void efi_file_load(EFI_File *f, FileProtocol *dir, const char *file_name) {
	FileProtocol *file = efi_open_file(dir, file_name);

	FileInfo info;
	UINTN info_size = sizeof(info);
	file->GetInfo(file, &kFileInfoGUID, &info_size, (uint8_t *)&info);

	UINTN buf_size = info.file_size;
	f->buf_pages = efi_allocate_pages(byte_size_to_page_size(buf_size));

	Status status = file->Read(file, &buf_size, f->buf_pages);
	efi_check_status(status, "Read failed.");

	f->file_size = info.file_size;
}
