#include "elf_loader.h"

#include <elf.h>

#include "console.h"
#include "efi_util.h"
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
	const Elf64_Ehdr *ehdr = ensure_loadable_(file->buf);
	if (!ehdr) panic("Invalid ELF.");

	for (int i = 0; i < ehdr->e_phnum; i++) {
		const Elf64_Phdr *phdr = (Elf64_Phdr *)(file->buf + ehdr->e_phoff + ehdr->e_phentsize * i);
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

void elf_load_kernel(EFI_File *file, ELFImage *out_image) {
	Elf64_Phdr *code;
	Elf64_Phdr *data;

	const Elf64_Ehdr *ehdr = parse_elf_(file, &code, &data);
	if (!ehdr) {
		panic("Can't load ELF.");
	}

	uint8_t *code_buf = efi_allocate_pages_addr(code->p_vaddr, byte_size_to_page_size(code->p_memsz));
	uint8_t *data_buf = efi_allocate_pages_addr(data->p_vaddr, byte_size_to_page_size(data->p_memsz));

	memcpy(code_buf, file->buf + code->p_offset, code->p_filesz);
	memcpy(data_buf, file->buf + data->p_offset, data->p_filesz);

	out_image->code = code_buf;
	out_image->data = data_buf;
	out_image->code_size = code->p_memsz;
	out_image->data_size = data->p_memsz;
	out_image->entry_point = (void *)ehdr->e_entry;
}
