#include "elf_loader.h"

#include <elf.h>

typedef struct PhdrInfo {
	const uint8_t *data;
	uint64_t vaddr;
	size_t map_size;
	size_t copy_size;
} PhdrInfo;

static const Elf64_Ehdr *ensure_loadable_(const uint8_t *buf) {
	if (memcmp(buf, ELFMAG, SELFMAG) != 0) {
		kpanic("Not an ELF file");
	}
	if (buf[EI_CLASS] != ELFCLASS64) {
		kpanic("Not an ELF Class 64n");
	}
	if (buf[EI_DATA] != ELFDATA2LSB) {
		kpanic("Not an ELF Data 2LSB");
	}
	if (buf[EI_OSABI] != ELFOSABI_SYSV) {
		kpanic("Not a SYSV ABI");
	}
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)(buf);
	if (ehdr->e_type != ET_EXEC) {
		kpanic("Not an executable");
	}
	if (ehdr->e_machine != EM_X86_64) {
		kpanic("Not for x86_64");
	}
	return ehdr;
}

static const Elf64_Ehdr *parse_elf_(uint8_t *buf, Elf64_Phdr **out_code, Elf64_Phdr **out_data) {
	const Elf64_Ehdr *ehdr = ensure_loadable_(buf);
	if (!ehdr) kpanic("Invalid ELF.");

	for (int i = 0; i < ehdr->e_phnum; i++) {
		Elf64_Phdr *phdr = (Elf64_Phdr *)(buf + ehdr->e_phoff + ehdr->e_phentsize * i);
		// klog("H %d %d %p %p %p", i, phdr->p_flags, phdr->p_memsz, phdr->p_vaddr, phdr->p_offset);
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

void elf_load_exec(uint8_t *buf, ELFImage *out_image) {
	Elf64_Phdr *code = NULL;
	Elf64_Phdr *data = NULL;

	const Elf64_Ehdr *ehdr = parse_elf_(buf, &code, &data);
	if (!ehdr) {
		kpanic("Can't load ELF.");
	}

	// klog("code %p %lld %p", code->p_vaddr, code->p_memsz, code->p_offset);
	// klog("data %p %lld %p", data->p_vaddr, data->p_memsz, data->p_offset);

#if 0
	uint8_t *code_buf = efi_allocate_pages_addr(code->p_vaddr, byte_size_to_page_size(code->p_memsz));
	uint8_t *data_buf = efi_allocate_pages_addr(data->p_vaddr, byte_size_to_page_size(data->p_memsz));

	memcpy(code_buf, file->buf + code->p_offset, code->p_filesz);
	memcpy(data_buf, file->buf + data->p_offset, data->p_filesz);

	out_image->code = code_buf;
	out_image->data = data_buf;
	out_image->code_size = code->p_memsz;
	out_image->data_size = data->p_memsz;
	out_image->entry_point = (void *)ehdr->e_entry;
#endif
}
