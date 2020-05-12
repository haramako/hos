#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifndef __GCC__ // For avoid VS Code assertion errors.
#define static_assert(...)
#endif

/// Packed struct specifier.
#define PACKED __attribute__((__packed__))

/// 'noreturn' function attribute
#define NORETURN __attribute__((__noreturn__))

#include "common_k.h"
