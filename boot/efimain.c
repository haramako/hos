#include "common.h"
#include "liumos.h"
#include "asm.h"
#include "efi.h"
#include "guid.h"

#include "efi_file.h"

#if 0
#include "corefunc.h"
#include "efi_file_manager.h"
#include "execution_context.h"
#include "hpet.h"
#include "liumos.h"
#endif

LiumOS* liumos;
EFI_MemoryMap efi_memory_map;
EFI_File liumos_elf_file;

LiumOS liumos_;
EFI efi_;
EFI_SystemTable *sys_;

void print(const char *c) {
	wchar_t buf[1024];
	int i=0;
	for( i=0; i<1024 && *c != '\0'; i++, c++){
		buf[i] = (wchar_t)*c;
	}
	buf[i] = u'\0';
	sys_->con_out->output_string(sys_->con_out, buf);
}

void print_hex_(uint64_t n) {
	char buf[19];
	for( int i=0; i<16; i++){
		int c = n & 0xf;
		buf[17-i] = (c >= 10) ? ('a' + c - 10) : ('0' + c);
		n = n >> 4;
	}
	buf[0] = '0';
	buf[1] = 'x';
	buf[18] = '\0';
	print(buf);
}

void print_hex(char *msg, uint64_t n) {
	print(msg);
	print_hex_(n);
	print("\n");
}

int memcmp(const void *a, const void *b, size_t size){
	uint8_t *a_ = a;
	uint8_t *b_ = b;
	for( size_t i = 0; i<size; i++){
		int cmp = b_[i] - a_[i];
		if( cmp != 0 ) return cmp;
	}
	return 0;
}

static const GUID kACPITableGUID = {
    0x8868e871,
    0xe4f1,
    0x11d3,
    {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};

void* GetConfigurationTableByUUID(const GUID* guid) {
	for (int i = 0; i < (int)sys_->number_of_table_entries; i++) {
		if (memcmp(guid, &sys_->configuration_table[i].vendor_guid, sizeof(GUID)) == 0 )
			return sys_->configuration_table[i].vendor_table;
	}
	return NULL;
}

void panic(){
	for(;;);
}

void efi_memory_map_init(EFI_MemoryMap *m) {
  uint32_t descriptor_version;
  m->bytes_used = sizeof(m->buf);
  print_hex("m ", (uint64_t)m);
  print_hex("bu ", (uint64_t)&m->bytes_used);
  print_hex("b ", (uint64_t)m->buf);
  print_hex("k ", m->key);
  print_hex("s ", m->descriptor_size);
  EFI_Status status = sys_->boot_services->GetMemoryMap(&m->bytes_used, m->buf, &m->key, &m->descriptor_size, &descriptor_version);
  if (status != Status_kSuccess) {
    print_hex("Failed to get memory map, status = ", status);
	panic();
  }
}

void efi_main(EFI_Handle image_handle, EFI_SystemTable* system_table) {
  liumos = &liumos_;
  sys_ = system_table;
  //efi_.Init(image_handle, system_table);
  liumos_.loader_info.efi = &efi_;

  liumos->acpi.rsdt = GetConfigurationTableByUUID(&kACPITableGUID);
  //assert(liumos->acpi.rsdt);

  print("\nStart bootloader.\n");

  print_hex("RSDT: ", (uint64_t)liumos->acpi.rsdt);

  //efi_.GetMemoryMapAndExitBootServices(image_handle, efi_memory_map);
  //liumos->efi_memory_map = &efi_memory_map;
  efi_memory_map_init(&efi_memory_map);
  EFI_Status status;

  //ACPI::DetectTables();

  //InitMemoryManagement(efi_memory_map);
  print("1\n");

  uint64_t kNumOfKernelStackPages = 2;
  //uint64_t kernel_stack_base =
  //    liumos->dram_allocator->AllocPages<uint64_t>(kNumOfKernelStackPages);
  //uint64_t kernel_stack_pointer =
  //    kernel_stack_base + (kNumOfKernelStackPages << kPageSizeExponent);

  print("4\n");
  //uint64_t ist1_base =
  //    liumos->dram_allocator->AllocPages<uint64_t>(kNumOfKernelStackPages);
  //uint64_t ist1_pointer =
  //    ist1_base + (kNumOfKernelStackPages << kPageSizeExponent);

  print("5\n");
  //InitPaging();

  //LoadKernelELF(liumos_elf_file);
  do {
	  status = sys_->boot_services->ExitBootServices(image_handle, efi_memory_map.key);
  } while( status != Status_kSuccess);
  
  for(;;);
}
