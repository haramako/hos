#include "syscall.h"

#include "console.h"
#include "gdt.h"

void syscall_init() {
	uint64_t star = ((uint64_t)kKernelCSSelector) << 32;
	star |= ((uint64_t)kUserCS32Selector) << 48;
	WriteMSR(MSRIndex_kSTAR, star);

	WriteMSR(MSRIndex_kLSTAR, (uint64_t)AsmSyscallHandler);
	WriteMSR(MSRIndex_kFMASK, 1ULL);

	IA32_EFER efer = {.data = ReadMSR(MSRIndex_kEFER)};
	efer.bits.syscall_enable = 1;
	WriteMSR(MSRIndex_kEFER, efer.data);
}

#define kSyscallIndex_sys_write 1
#define kSyscallIndex_sys_exit 60
#define kSyscallIndex_arch_prctl 158

__attribute__((ms_abi)) void SyscallHandler(uint64_t *args) {
	uint64_t idx = args[0];
	if (idx == kSyscallIndex_sys_write) {
		const uint64_t fildes = args[1];
		const char *buf = (char *)args[2];
		uint64_t nbyte = args[3];
		if (fildes != 1) {
			kpanic("Only stdout is supported for now.");
		}
		console_write(buf);
		return;
	} else if (idx == kSyscallIndex_sys_exit) {
	} else {
		kpanic("Unknown syscall!");
	}
}
