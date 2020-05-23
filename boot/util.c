#include "util.h"

#include "console.h"

int memcmp(const void *a, const void *b, size_t size) {
	uint8_t *a_ = (uint8_t *)a;
	uint8_t *b_ = (uint8_t *)b;
	for (size_t i = 0; i < size; i++) {
		int cmp = b_[i] - a_[i];
		if (cmp != 0) return cmp;
	}
	return 0;
}

void *memcpy(void *dest, const void *src, size_t size) {
	uint8_t *dest_ = dest;
	uint8_t *src_ = src;
	for (size_t i = 0; i < size; i++) {
		*dest_++ = *src_++;
	}
	return dest;
}

void *memset(void *dest, int n, size_t size) {
	uint8_t *dest_ = dest;
	for (size_t i = 0; i < size; i++) {
		*dest_++ = n;
	}
	return dest;
}

size_t byte_size_to_page_size(size_t size) { return (size + 4095) / 4096; }

wchar_t *str2wstr(wchar_t *dest, const char *src, size_t size) {
	wchar_t *orig_dest = dest;
	for (size_t i = 0; i < size && *src != '\0'; i++) {
		*dest++ = *src++;
	}
	*dest = '\0';
	return orig_dest;
}
