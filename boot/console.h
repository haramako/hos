#pragma once

#include "common.h"

#include "efi.h"

void print(const char *c);
void print_hex_(uint64_t n);
void print_hex(char *msg, uint64_t n);
NORETURN void panic(const char *msg);
