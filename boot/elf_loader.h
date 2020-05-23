#pragma once

#include "common.h"

#include "efi_util.h"
#include "liumos.h"

void elf_load_kernel(EFI_File *file, LiumOS *liumos);
