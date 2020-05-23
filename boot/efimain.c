#include "asm.h"
#include "common.h"
#include "console.h"
#include "efi.h"
#include "guid.h"
#include "liumos.h"

#include "efi_file.h"

LiumOS *liumos;
EFI_MemoryMap efi_memory_map;
EFI_File liumos_elf_file;

LiumOS liumos_;

static const GUID kACPITableGUID = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};

void *GetConfigurationTableByUUID(const GUID *guid) {
	for (int i = 0; i < (int)sys_->number_of_table_entries; i++) {
		if (memcmp(guid, &sys_->configuration_table[i].vendor_guid, sizeof(GUID)) == 0)
			return sys_->configuration_table[i].vendor_table;
	}
	return NULL;
}

void efi_memory_map_init(EFI_MemoryMap *m) {
	uint32_t descriptor_version;
	m->bytes_used = sizeof(m->buf);
	EFI_Status status =
		sys_->boot_services->GetMemoryMap(&m->bytes_used, m->buf, &m->key, &m->descriptor_size, &descriptor_version);
	if (status != Status_kSuccess) {
		print_hex("Failed to get memory map, status = ", status);
		panic("");
	}
}

void efi_main(EFI_Handle image_handle, EFI_SystemTable *system_table) {
	liumos = &liumos_;
	g_image_handle = image_handle, sys_ = system_table;
	// efi_.Init(image_handle, system_table);
	liumos_.loader_info.efi = &efi_;

	liumos->acpi.rsdt = GetConfigurationTableByUUID(&kACPITableGUID);
	// assert(liumos->acpi.rsdt);

	print("\nStart bootloader.\n");

	print_hex("RSDT: ", (uint64_t)liumos->acpi.rsdt);

	liumos->efi_memory_map = &efi_memory_map;
	efi_memory_map_init(&efi_memory_map);
	EFI_Status status;

	FileProtocol *root = efi_file_root();
	efi_file_load(&liumos_elf_file, root, "LIUMOS.ELF");
	liumos_.loader_info.files.liumos_elf = &liumos_elf_file;

	uint64_t kNumOfKernelStackPages = 2;
	// uint64_t kernel_stack_base =
	//    liumos->dram_allocator->AllocPages<uint64_t>(kNumOfKernelStackPages);
	// uint64_t kernel_stack_pointer =
	//    kernel_stack_base + (kNumOfKernelStackPages << kPageSizeExponent);

	print("4\n");
	// uint64_t ist1_base =
	//    liumos->dram_allocator->AllocPages<uint64_t>(kNumOfKernelStackPages);
	// uint64_t ist1_pointer =
	//    ist1_base + (kNumOfKernelStackPages << kPageSizeExponent);

	// LoadKernelELF(liumos_elf_file);
	do {
		status = sys_->boot_services->ExitBootServices(image_handle, efi_memory_map.key);
	} while (status != Status_kSuccess);

	for (;;)
		;
}
