#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "newlib_func.h"

//! Packed struct specifier.
#define PACKED __attribute__((__packed__))

//! Page size by byte.
#define PAGE_SIZE 4096
