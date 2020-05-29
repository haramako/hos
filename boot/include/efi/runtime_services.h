#pragma once

#include <assert.h>
#include <stdint.h>

typedef enum { EfiResetCold, EfiResetWarm, EfiResetShutdown, EfiResetPlatformSpecific } ResetType;

typedef struct EFI_RuntimeServices_ {
    char _buf_rs1[24];
    uint64_t _buf_rs2[10];
    __attribute__((__ms_abi__)) void (*reset_system)(ResetType,
                         uint64_t reset_status,
                         uint64_t data_size,
                         void*);
} EFI_RuntimeServices;
