#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include <assert.h> // Must include after stdint.h

#ifndef __clang__ // For avoid VS Code assertion errors.
#define static_assert(...)
#endif

/// Packed struct specifier.
#define PACKED __attribute__((__packed__))

/// 'noreturn' function attribute
#define NORETURN __attribute__((__noreturn__))
