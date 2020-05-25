#include "boot_param.h"
#include "common.h"
#include "console.h"
#include "efi.h"

#include "efi_util.h"
#include "elf_loader.h"
#include "util.h"

EFI_File liumos_elf_file;

BootParam boot_param_;

static const GUID kACPITableGUID = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};

void *GetConfigurationTableByUUID(const GUID *guid) {
	for (int i = 0; i < (int)sys_->number_of_table_entries; i++) {
		if (memcmp(guid, &sys_->configuration_table[i].vendor_guid, sizeof(GUID)) == 0)
			return sys_->configuration_table[i].vendor_table;
	}
	return NULL;
}

void efi_memory_map_init(EFI_MemoryMap *m) {
	m->bytes_used = sizeof(m->buf);
	Status status =
		sys_->boot_services->GetMemoryMap(&m->bytes_used, m->buf, &m->key, &m->descriptor_size, &m->descriptor_version);
	if (status != Status_kSuccess) {
		print_hex("Failed to get memory map, status = ", status);
		panic("");
	}
}

void efi_main(Handle image_handle, SystemTable *system_table) {
	g_image_handle = image_handle;
	sys_ = system_table;

	boot_param_.rsdt = GetConfigurationTableByUUID(&kACPITableGUID);

	print("\nStart bootloader.\n");

	print_hex("RSDT: ", (uint64_t)boot_param_.rsdt);

	boot_param_.efi_memory_map = &g_efi_memory_map;
	efi_memory_map_init(&g_efi_memory_map);
	Status status;

	FileProtocol *root = efi_file_root();
	efi_file_load(&liumos_elf_file, root, "LIUMOS.ELF");

	print_hex("file buf: ", (uint64_t)liumos_elf_file.buf_pages);
	print_hex("file: ", *((uint64_t *)liumos_elf_file.buf_pages));

	elf_load_kernel(&liumos_elf_file, &boot_param_);

	do {
		status = sys_->boot_services->ExitBootServices(image_handle, g_efi_memory_map.key);
	} while (status != Status_kSuccess);

	for (;;)
		;
}
