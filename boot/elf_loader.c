#include "elf_loader.h"

#include <elf.h>

#include "console.h"
#include "efi_util.h"
#include "page.h"
#include "util.h"

typedef struct PhdrInfo {
	const uint8_t *data;
	uint64_t vaddr;
	size_t map_size;
	size_t copy_size;
} PhdrInfo;

static const Elf64_Ehdr *ensure_loadable_(const uint8_t *buf) {
	if (memcmp(buf, ELFMAG, SELFMAG) != 0) {
		panic("Not an ELF file");
	}
	if (buf[EI_CLASS] != ELFCLASS64) {
		panic("Not an ELF Class 64n");
	}
	if (buf[EI_DATA] != ELFDATA2LSB) {
		panic("Not an ELF Data 2LSB");
	}
	if (buf[EI_OSABI] != ELFOSABI_SYSV) {
		panic("Not a SYSV ABI");
	}
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)(buf);
	if (ehdr->e_type != ET_EXEC) {
		panic("Not an executable");
	}
	if (ehdr->e_machine != EM_X86_64) {
		panic("Not for x86_64");
	}
	return ehdr;
}

static const Elf64_Ehdr *parse_elf_(EFI_File *file, Elf64_Phdr **out_code, Elf64_Phdr **out_data) {
	const uint8_t *buf = (uint8_t *)file->buf_pages;
	const Elf64_Ehdr *ehdr = ensure_loadable_(buf);
	if (!ehdr) panic("Invalid ELF.");

	for (int i = 0; i < ehdr->e_phnum; i++) {
		const Elf64_Phdr *phdr = (Elf64_Phdr *)(buf + ehdr->e_phoff + ehdr->e_phentsize * i);
		if (phdr->p_type != PT_LOAD) continue;

		if (phdr->p_flags & PF_X) {
			*out_code = phdr;
		}
		if (phdr->p_flags & PF_W) {
			*out_data = phdr;
		}
	}
	return ehdr;
}

void elf_load_kernel(EFI_File *file) {
	Elf64_Phdr *code;
	Elf64_Phdr *data;

	const Elf64_Ehdr *ehdr = parse_elf_(file, &code, &data);
	if (!ehdr) {
		panic("Can't load ELF.");
	}

	uint8_t *code_buf = efi_allocate_pages_addr(code->p_vaddr, byte_size_to_page_size(code->p_filesz));
	uint8_t *data_buf = efi_allocate_pages_addr(data->p_vaddr, byte_size_to_page_size(data->p_filesz));

	print_hex("code ", code->p_filesz);
	print_hex("data ", data->p_filesz);
	memcpy(code_buf, (uint8_t *)file->buf_pages + code->p_offset, code->p_filesz);
	memcpy(data_buf, (uint8_t *)file->buf_pages + data->p_offset, data->p_filesz);

	void *entry_point = (void *)ehdr->e_entry;
	print_hex("Entry address: ", (uint64_t)entry_point);

	// JumpToKernel(entry_point, liumos, kernel_main_stack_pointer);
}
