#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// In stdlib.h
void *malloc(size_t size);
void *calloc(size_t n, size_t size);
void free(void *p);

#ifndef __clang__ // For avoid VS Code assertion errors.
#define static_assert(...)
#endif

/// Packed struct specifier.
#define PACKED __attribute__((__packed__))

/// 'noreturn' function attribute
#define NORETURN __attribute__((__noreturn__))

#include "common_k.h"
