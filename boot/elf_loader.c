#include "elf_loader.h"

#include <elf.h>

#include "asm.h"
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

void efi_memory_map_init2(EFI_MemoryMap *m) {
	m->bytes_used = sizeof(m->buf);
	EFI_Status status =
		sys_->boot_services->GetMemoryMap(&m->bytes_used, m->buf, &m->key, &m->descriptor_size, &m->descriptor_version);
	if (status != Status_kSuccess) {
		print_hex("Failed to get memory map, status = ", status);
		panic("");
	}
}

typedef struct Serial {
	uint16_t port;
} Serial;

void serial_init();
Serial *serial_get_port(int serial_num);
void serial_new(Serial *s, uint16_t port);
void serial_send_char(Serial *s, char c);

typedef void (*entry_point_t)();

void elf_load_kernel(EFI_File *file, LiumOS *liumos) {
	Elf64_Phdr *code;
	Elf64_Phdr *data;

	const Elf64_Ehdr *ehdr = parse_elf_(file, &code, &data);
	if (!ehdr) {
		panic("Can't load ELF.");
	}

	uint8_t *code_buf = efi_allocate_pages_addr(code->p_vaddr, byte_size_to_page_size(code->p_filesz));
	uint8_t *data_buf = efi_allocate_pages_addr(data->p_vaddr, byte_size_to_page_size(data->p_memsz));

	uint64_t cr0 = asm_read_cr0();
	print_hex("cr0 ", cr0);
	cr0 &= ~(1 << 16);
	asm_write_cr0(cr0);
	print_hex("cr0 ", asm_read_cr0());

	print_hex("func ", (int64_t)elf_load_kernel);
	print_hex("code ", code->p_filesz);
	print_hex("code ", code->p_memsz);
	print_hex("data ", data->p_filesz);
	print_hex("data ", data->p_memsz);

	memcpy(code_buf, (uint8_t *)file->buf_pages + code->p_offset, code->p_filesz);
	memcpy(data_buf, (uint8_t *)file->buf_pages + data->p_offset, data->p_filesz);

	void *entry_point = (void *)ehdr->e_entry;
	print_hex("Entry address: ", (uint64_t)entry_point);
	print_hex("      head   : ", *(uint64_t *)entry_point);

	print("hoge\n");

	efi_memory_map_init2(&g_efi_memory_map);
	print("5\n");
	// print_hex("mdesc ", g_efi_memory_map.descriptor_size);

	liumos->efi_memory_map = &g_efi_memory_map;

	Status status;
	do {
		status = sys_->boot_services->ExitBootServices(g_image_handle, g_efi_memory_map.key);
	} while (status != Status_kSuccess);

	JumpToKernel(entry_point, liumos, 0);
	print("hoge\n");
}

static Serial com_[2];

void serial_init() {
	serial_new(&com_[0], 0x3f8);
	serial_new(&com_[1], 0x2f8);
}

Serial *serial_get_port(int serial_num) { return &com_[serial_num]; }

void serial_new(Serial *s, uint16_t port) {
	// https://wiki.osdev.org/Serial_Ports
	s->port = port;
	WriteIOPort8(s->port + 1, 0x00);	  // Disable all interrupts
	WriteIOPort8(s->port + 3, 0x80);	  // Enable DLAB (set baud rate divisor)
	const uint16_t baud_divisor = 0x0001; // baud rate = (115200 / baud_divisor)
	WriteIOPort8(s->port + 0, baud_divisor & 0xff);
	WriteIOPort8(s->port + 1, baud_divisor >> 8);
	WriteIOPort8(s->port + 3, 0x03); // 8 bits, no parity, one stop bit
	WriteIOPort8(s->port + 2,
				 0xC7);				 // Enable FIFO, clear them, with 14-byte threshold
	WriteIOPort8(s->port + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

bool serial_is_transmit_empty(Serial *s) { return ReadIOPort8(s->port + 5) & 0x20; }

void serial_send_char(Serial *s, char c) {
	while (!serial_is_transmit_empty(s))
		;
	WriteIOPort8(s->port, c);
}
