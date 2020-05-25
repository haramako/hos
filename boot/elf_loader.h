#pragma once

#include "common.h"

#include "boot_param.h"
#include "efi_util.h"

void elf_load_kernel(EFI_File *file, LiumOS *liumos);
