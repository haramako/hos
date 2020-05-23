#pragma once

#include "common.h"

#include "efi.h"

extern EFI efi_;
extern EFI_Handle g_image_handle;
extern EFI_SystemTable *sys_;

void print(const char *c);
void print_hex_(uint64_t n);
void print_hex(char *msg, uint64_t n);
int memcmp(const void *a, const void *b, size_t size);
void panic(const char *msg);

size_t byte_size_to_page_size(size_t size);
wchar_t *str2wstr(wchar_t *dest, const char *src, size_t size);

void check_status(Status status, const char *msg);
