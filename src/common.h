#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "newlib_func.h"

//! Packed struct specifier.
#define PACKED __attribute__((__packed__))

//! static_assert
#define static_assert(x) _Static_assert(x,"static_assert failed.")

//! Page size by byte.
#define PAGE_SIZE 4096
