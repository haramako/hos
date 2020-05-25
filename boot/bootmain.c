#include "asm.h"
#include "boot_param.h"
#include "console.h"
#include "efi.h"
#include "efi_util.h"
#include "elf_loader.h"
#include "util.h"

EFI_File kernel_elf_file;
EFI_File bootfs_file;

BootParam boot_param_;
EFI_MemoryMap efi_memory_map_;

static const GUID kACPITableGUID = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};

static const GUID kGraphicsOutputProtocolGUID = {
	0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};

void *GetConfigurationTableByUUID(const GUID *guid) {
	for (int i = 0; i < (int)g_sys->number_of_table_entries; i++) {
		if (memcmp(guid, &g_sys->configuration_table[i].vendor_guid, sizeof(GUID)) == 0)
			return g_sys->configuration_table[i].vendor_table;
	}
	return NULL;
}

void efi_memory_map_init(EFI_MemoryMap *m) {
	m->bytes_used = sizeof(m->buf);
	Status status = g_sys->boot_services->GetMemoryMap(&m->bytes_used, m->buf, &m->key, &m->descriptor_size,
													   &m->descriptor_version);
	if (status != Status_kSuccess) {
		print_hex("Failed to get memory map, status = ", status);
		panic("");
	}
}

void efi_main(Handle image_handle, SystemTable *system_table) {
	Status status;

	g_image_handle = image_handle;
	g_sys = system_table;

	boot_param_.rsdt = GetConfigurationTableByUUID(&kACPITableGUID);

	print("\nStart bootloader.\n");

	// print_hex("RSDT: ", (uint64_t)boot_param_.rsdt);

	// Get graphics info.
	GraphicsOutputProtocol *graphics = (GraphicsOutputProtocol *)efi_locate_protocol(&kGraphicsOutputProtocolGUID);
	// print_hex("Graphics: ", (uint64_t)graphics);
	boot_param_.graphics.vram = (void *)graphics->mode->frame_buffer_base;
	boot_param_.graphics.width = graphics->mode->info->horizontal_resolution;
	boot_param_.graphics.height = graphics->mode->info->vertical_resolution;
	boot_param_.graphics.pixels_per_scan_line = graphics->mode->info->pixels_per_scan_line;

	// Load kernel elf.
	FileProtocol *root = efi_file_root();
	efi_file_load(&kernel_elf_file, root, "LIUMOS.ELF");

	// print_hex("Kernel image: ", (uint64_t)liumos_elf_file.buf_pages);
	// print_hex("file: ", *((uint64_t *)liumos_elf_file.buf_pages));

	ELFImage elf_image;
	elf_load_kernel(&kernel_elf_file, &elf_image);

	// Load bootfs.
	efi_file_load(&bootfs_file, root, "BOOTFS.IMG");

	boot_param_.bootfs_buf = bootfs_file.buf;
	boot_param_.bootfs_size = bootfs_file.file_size;

	efi_memory_map_init(&efi_memory_map_);
	boot_param_.efi_memory_map = &efi_memory_map_;

	do {
		status = g_sys->boot_services->ExitBootServices(g_image_handle, efi_memory_map_.key);
	} while (status != Status_kSuccess);

	asm_jump_to_kernel(elf_image.entry_point, &boot_param_, 0);

	panic("Must not return from kernel.");
}
