#include "console.h"

EFI efi_;
EFI_Handle g_image_handle;
EFI_SystemTable *sys_;

void print(const char *c) {
	wchar_t buf[1024];
	int i = 0;
	for (i = 0; i < 1024 && *c != '\0'; i++, c++) {
		buf[i] = (wchar_t)*c;
	}
	buf[i] = u'\0';
	sys_->con_out->output_string(sys_->con_out, buf);
}

void print_hex_(uint64_t n) {
	char buf[19];
	for (int i = 0; i < 16; i++) {
		int c = n & 0xf;
		buf[17 - i] = (c >= 10) ? ('a' + c - 10) : ('0' + c);
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

int memcmp(const void *a, const void *b, size_t size) {
	uint8_t *a_ = (uint8_t *)a;
	uint8_t *b_ = (uint8_t *)b;
	for (size_t i = 0; i < size; i++) {
		int cmp = b_[i] - a_[i];
		if (cmp != 0) return cmp;
	}
	return 0;
}

void panic(const char *msg) {
	print(msg);
	print("\n");
	for (;;)
		;
}

size_t byte_size_to_page_size(size_t size) { return size / 4096; }

wchar_t *str2wstr(wchar_t *dest, const char *src, size_t size) {
	wchar_t *orig_dest = dest;
	for (size_t i = 0; i < size && *src != '\0'; i++) {
		*dest++ = *src++;
	}
	*dest = '\0';
	return orig_dest;
}

void check_status(Status status, const char *msg) {
	if (status != Status_kSuccess) {
		panic(msg);
	}
}
