#include "efi_file.h"

#include "console.h"
#include "efi.h"

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

FileProtocol *efi_open_file(FileProtocol *dir, char *path) {
	wchar_t wpath[kFileNameSize];
	FileProtocol *file = NULL;
	Status status = dir->Open(dir, &file, str2wstr(wpath, path, kFileNameSize), kRead, 0);
	if (status != Status_kSuccess || !file) {
		print("filename: ");
#if 0
		while (path && *path) {
			PutChar(*path);
			path++;
		}
#endif
		if (status != Status_kSuccess) {
			panic("Failed to open file.");
		}
	}
	return file;
}

#if 0
void strncpy(char *dest, const char *src, size_t size){
	for( size_t i=0; i<size && *src != '\0'; i++){
		*dest++ = *src++;
	}
	*dest = '\0';
}
#endif

void *efi_allocate_pages(size_t pages) { return NULL; }

Status efi_handle_protocol(Handle handle, GUID *guid, void **out) {
	return sys_->boot_services->HandleProtocol(g_image_handle, guid, out);
}

FileProtocol *efi_file_root() {
	Status status;

	LoadedImageProtocol *loaded_image;
	status = efi_handle_protocol(g_image_handle, &kLoadedImageProtocolGUID, (void **)&loaded_image);
	check_status(status, "Can't get LoadedImageProtocol.");

	SimpleFileSystemProtocol *simple_fs;
	status = sys_->boot_services->HandleProtocol(loaded_image->device_handle, &kSimpleFileSystemProtocolGUID,
												 (void **)&simple_fs);
	check_status(status, "Can't get SimpleFileSystemProtocol.");

	FileProtocol *root;
	simple_fs->OpenVolume(simple_fs, &root);
	check_status(status, "OpenVolume() failed.");
	return root;
}

void efi_file_load(EFI_File *f, FileProtocol *dir, const char *file_name) {
	FileProtocol *file = efi_open_file(dir, file_name);

	FileInfo info;
	UINTN info_size = sizeof(info);
	file->GetInfo(file, &kFileInfoGUID, &info_size, (uint8_t *)&info);

	UINTN buf_size = info.file_size;
	void *buf_pages = efi_allocate_pages(byte_size_to_page_size(buf_size));

	Status status = file->Read(file, &buf_size, buf_pages);
	check_status(status, "Read failed.");

	// assert(buf_size == info.file_size);
	f->file_size = info.file_size;
}
