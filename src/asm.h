#pragma once

#include "common.h"

// CPUIDIndex
#define CPUIDIndex_kXTopology 0x0B
#define CPUIDIndex_kMaxAddr 0x80000008

#define kCPUID01H_EDXBitAPIC (1 << 9)
#define kCPUID01H_ECXBitx2APIC (1 << 21)
#define kCPUID01H_EDXBitMSR (1 << 5)
#define kIOAPICRegIndexAddr 0xfec00000
#define kIOAPICRegDataAddr (kIOAPICRegIndexAddr + 0x10)
#define kLocalAPICBaseBitAPICEnabled (1 << 11)
#define kLocalAPICBaseBitx2APICEnabled (1 << 10)

#define kRFlagsInterruptEnable (1ULL << 9)

typedef struct PACKED CPUFeatureSet {
	uint64_t max_phy_addr;
	uint64_t phy_addr_mask;				 // = (1ULL << max_phy_addr) - 1
	uint64_t kernel_phys_page_map_begin; // = ~((1ULL << (max_phy_addr - 1) - 1))
	uint32_t max_cpuid;
	uint32_t max_extended_cpuid;
	uint8_t family, model, stepping;
	bool x2apic;
	bool clfsh;
	bool clflushopt;
	char brand_string[48];
} CPUFeatureSet;

typedef struct PACKED CPUID {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} CPUID;

typedef struct PACKED GDTR {
	uint16_t limit;
	uint64_t *base;
} GDTR;

typedef struct PACKED GeneralRegisterContext {
	uint64_t rax;
	uint64_t rdx;
	uint64_t rbx;
	uint64_t rbp;
	uint64_t rsi;
	uint64_t rdi;
	//
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	//
	uint64_t rcx;
} GeneralRegisterContext;
static_assert(sizeof(GeneralRegisterContext) == (16 - 1) * 8, "Invalid size.");

typedef struct PACKED InterruptContext {
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} InterruptContext;
static_assert(sizeof(InterruptContext) == 40, "Invalid size.");

typedef struct PACKED CPUContext {
	uint64_t cr3;
	GeneralRegisterContext greg;
	InterruptContext int_ctx;
} CPUContext;

typedef struct PACKED InterruptInfo {
	GeneralRegisterContext greg;
	uint64_t error_code;
	InterruptContext int_ctx;
} InterruptInfo;
static_assert(sizeof(InterruptInfo) == (16 + 4 + 1) * 8, "Invalid size.");

typedef enum {
	kInterruptGate = 0xE,
	kTrapGate = 0xF,
} IDTType;

typedef struct PACKED IDTGateDescriptor {
	uint16_t offset_low;
	uint16_t segment_descriptor;
	unsigned interrupt_stack_table : 3;
	unsigned reserved0 : 5;
	unsigned type : 4;
	unsigned reserved1 : 1;
	unsigned descriptor_privilege_level : 2;
	unsigned present : 1;
	unsigned offset_mid : 16;
	uint32_t offset_high;
	uint32_t reserved2;
} IDTGateDescriptor;

typedef struct PACKED IDTR {
	uint16_t limit;
	IDTGateDescriptor *base;
} IDTR;

#if 0
struct PACKED IA32_EFER_BITS {
  unsigned syscall_enable : 1;
  unsigned reserved0 : 7;
  unsigned LME : 1;
  unsigned reserved1 : 1;
  unsigned LMA : 1;
  unsigned NXE : 1;
};

struct PACKED IA32_EFER {
  union {
    uint64_t data;
    IA32_EFER_BITS bits;
  };
};

struct PACKED IA32_MaxPhyAddr_BITS {
  uint8_t physical_address_bits;
  uint8_t linear_address_bits;
};

struct PACKED IA32_MaxPhyAddr {
  union {
    uint64_t data;
    IA32_MaxPhyAddr_BITS bits;
  };
};

struct PACKED IA_CR3_BITS {
  uint64_t ignored0 : 3;
  uint64_t PWT : 1;
  uint64_t PCD : 1;
  uint64_t ignored1 : 7;
  uint64_t pml4_addr : 52;
};

#endif

typedef struct PACKED IA_TSS64 {
	uint32_t reserved0;
	uint64_t rsp[3];
	uint64_t ist[9];
	uint16_t reserved1;
	uint16_t io_map_base_addr_ofs;
} IA_TSS64;
static_assert(sizeof(IA_TSS64) == 104, "Invalid IA_TSS64 size");

typedef uint64_t MSRIndex;
#define MSRIndex_kLocalAPICBase 0x1b
#define MSRIndex_kx2APICEndOfInterrupt 0x80b
#define MSRIndex_kEFER 0xC0000080
#define MSRIndex_kSTAR 0xC0000081
#define MSRIndex_kLSTAR 0xC0000082
#define MSRIndex_kFMASK 0xC0000084
#define MSRIndex_kFSBase 0xC0000100
#define MSRIndex_kKernelGSBase 0xC0000102p

__attribute__((ms_abi)) void Sleep(void);
__attribute__((ms_abi)) void ReadCPUID(CPUID *, uint32_t eax, uint32_t ecx);

__attribute__((ms_abi)) uint64_t ReadMSR(MSRIndex);
__attribute__((ms_abi)) void WriteMSR(MSRIndex, uint64_t);

__attribute__((ms_abi)) void ReadGDTR(GDTR *);
__attribute__((ms_abi)) void WriteGDTR(GDTR *);
__attribute__((ms_abi)) void ReadIDTR(IDTR *);
__attribute__((ms_abi)) void WriteIDTR(IDTR *);
__attribute__((ms_abi)) void WriteTaskRegister(uint16_t);
__attribute__((ms_abi)) void Int03(void);
__attribute__((ms_abi)) uint8_t ReadIOPort8(uint16_t);
__attribute__((ms_abi)) void WriteIOPort8(uint16_t, uint8_t);
__attribute__((ms_abi)) uint32_t ReadIOPort32(uint16_t);
__attribute__((ms_abi)) void WriteIOPort32(uint16_t addr, uint32_t data);
__attribute__((ms_abi)) void StoreIntFlag(void);
__attribute__((ms_abi)) void StoreIntFlagAndHalt(void);
__attribute__((ms_abi)) void ClearIntFlag(void);
__attribute__((noreturn)) __attribute__((ms_abi)) void Die(void);
__attribute__((ms_abi)) uint16_t ReadCSSelector(void);
__attribute__((ms_abi)) uint16_t ReadSSSelector(void);
__attribute__((ms_abi)) void WriteCSSelector(uint16_t);
__attribute__((ms_abi)) void WriteSSSelector(uint16_t);
__attribute__((ms_abi)) void WriteDataAndExtraSegmentSelectors(uint16_t);
__attribute__((ms_abi)) uint64_t ReadCR2(void);
__attribute__((ms_abi)) uint64_t ReadCR3(void);
__attribute__((ms_abi)) void WriteCR3(uint64_t);
__attribute__((ms_abi)) uint64_t CompareAndSwap(uint64_t *, uint64_t);
__attribute__((ms_abi)) void SwapGS(void);
__attribute__((ms_abi)) uint64_t ReadRSP(void);
__attribute__((ms_abi)) void ChangeRSP(uint64_t);
__attribute__((ms_abi)) void RepeatMoveBytes(size_t count, const void *dst, const void *src);
__attribute__((ms_abi)) void RepeatMove4Bytes(size_t count, const void *dst, const void *src);
__attribute__((ms_abi)) void RepeatStore4Bytes(size_t count, const void *dst, uint32_t data);
__attribute__((ms_abi)) void RepeatMove8Bytes(size_t count, const void *dst, const void *src);
__attribute__((ms_abi)) void RepeatStore8Bytes(size_t count, const void *dst, uint64_t data);
__attribute__((ms_abi)) void CLFlushOptimized(const void *);
__attribute__((ms_abi)) void JumpToKernel(void *kernel_entry_point, void *vram_sheet, uint64_t kernel_stack_pointer);
__attribute__((ms_abi)) void AsmSyscallHandler(void);
__attribute__((ms_abi)) void AsmIntHandler00_DivideError(void);
__attribute__((ms_abi)) void AsmIntHandler03(void);
__attribute__((ms_abi)) void AsmIntHandler06(void);
__attribute__((ms_abi)) void AsmIntHandler07_DeviceNotAvailable(void);
__attribute__((ms_abi)) void AsmIntHandler08(void);
__attribute__((ms_abi)) void AsmIntHandler0D(void);
__attribute__((ms_abi)) void AsmIntHandler0E(void);
__attribute__((ms_abi)) void AsmIntHandler10_x87FPUError(void);
__attribute__((ms_abi)) void AsmIntHandler13_SIMDFPException(void);
__attribute__((ms_abi)) void AsmIntHandler20(void);
__attribute__((ms_abi)) void AsmIntHandler21(void);
__attribute__((ms_abi)) void AsmIntHandlerNotImplemented(void);
__attribute__((ms_abi)) void Disable8259PIC(void);
