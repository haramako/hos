#include "console.h"

#include "efi_util.h"

void print(const char *c) {
	wchar_t buf[257];
	int i = 0;
	for (i = 0; i < 256 && *c != '\0'; i++, c++) {
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

NORETURN void panic(const char *msg) {
	print(msg);
	print("\n");
	for (;;)
		;
}
