#include "common_k.h"

void kpanic(const char *msg, ...) {
	va_list vargs;
	va_start(vargs, msg);
	console_vprintf(msg, vargs);
	va_end(vargs);
	while (1) {
	}
}

const char *humanize_size(uint64_t size) {
	static char buf[16];
	if (size >= (1LLU << 30)) {
		snprintf(buf, sizeof(buf), "%4ld GB", size >> 30);
	} else if (size >= (1LLU << 20)) {
		snprintf(buf, sizeof(buf), "%4ld MB", size >> 20);
	} else if (size >= (1LLU << 10)) {
		snprintf(buf, sizeof(buf), "%4ld KB", size >> 10);
	} else {
		snprintf(buf, sizeof(buf), "%ld", size);
	}
	return buf;
}

char to_hex(char n) {
	assert(n >= 0 && n < 16);
	if (n < 10) {
		return '0' + n;
	} else {
		return 'a' + (n - 10);
	}
}

char *dump_bytes(void *p_, size_t size) {
	assert(size < 0x10000);

	static char buf[8000];
	char *s = buf;
	uint8_t *p = (uint8_t *)p_;

	for (size_t i = 0; i < size; i++) {
		if (i % 16 == 0) {
			s += snprintf(s, 8, "%04x: ", (int)i);
		}
		*(s++) = to_hex(p[i] >> 4);
		*(s++) = to_hex(p[i] & 0x0f);
		*(s++) = ' ';
		if (i % 16 == 15) {
			*(s++) = '\n';
		}
	}

	*s++ = '\0';
	return buf;
}

extern inline uint64_t int_merge64(uint32_t high, uint32_t low);
extern inline uint32_t uint64_high(uint64_t n);
extern inline uint32_t uint64_low(uint64_t n);
