#pragma once

#include "common.h"

#include "efi.h"

int memcmp(const void *a, const void *b, size_t size);
void *memcpy(void *dest, const void *src, size_t size);
void *memset(void *dest, int n, size_t size);
size_t byte_size_to_page_size(size_t size);
wchar_t *str2wstr(wchar_t *dest, const char *src, size_t size);
