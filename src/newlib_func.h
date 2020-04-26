#pragma once

#include <stddef.h>
#include <stdint.h>

int strncmp(const char* s1, const char* s2, size_t n);
int strcmp(const char* s1, const char* s2);
void* memcpy(void* dst, const void* src, size_t n);
void bzero(volatile void* s, size_t n);
int atoi(const char* str);
