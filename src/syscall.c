#include "syscall.h"

#include "console.h"
#include "gdt.h"

#include "process.h"
#include "scheduler.h"

#define SYSCALL_WRITE 1
#define SYSCALL_EXIT 60
#define SYSCALL_MAX 255

typedef void (*SyscallHandlerFunc)(uint64_t *args);

void syscall_write(uint64_t *args);
void syscall_exit(uint64_t *args);

SyscallHandlerFunc syscall_handlers_[SYSCALL_MAX];

void syscall_init() {
	uint64_t star = ((uint64_t)kKernelCSSelector) << 32;
	star |= ((uint64_t)kUserCS32Selector) << 48;
	WriteMSR(MSRIndex_kSTAR, star);

	WriteMSR(MSRIndex_kLSTAR, (uint64_t)AsmSyscallHandler);
	WriteMSR(MSRIndex_kFMASK, 1ULL);

	IA32_EFER efer = {.data = ReadMSR(MSRIndex_kEFER)};
	efer.bits.syscall_enable = 1;
	WriteMSR(MSRIndex_kEFER, efer.data);

	// Initialize syscall handlers.
	memset(syscall_handlers_, 0, sizeof(syscall_handlers_));
	syscall_handlers_[SYSCALL_WRITE] = syscall_write;
	syscall_handlers_[SYSCALL_EXIT] = syscall_exit;
}

__attribute__((ms_abi)) void SyscallHandler(uint64_t *args) {
	uint64_t idx = args[0];

	if (idx > SYSCALL_MAX) {
		kpanic("Unknown syscall!");
	}

	SyscallHandlerFunc handler = syscall_handlers_[idx];
	if (!handler) {
		kpanic("Unknown syscall!");
	}
	handler(args);
}

void syscall_write(uint64_t *args) {
	const uint64_t fd = args[1];
	const char *buf = (char *)args[2];
	uint64_t nbyte = args[3];
	// klog("write %ld, %p, %ld", fd, buf, nbyte);
	if (fd != 1) {
		kpanic("Only stdout is supported for now.");
	}
	console_write(buf);
}

void syscall_exit(uint64_t *args) {
	const uint64_t exit_code = args[1];
	Process *p = scheduler_current_process();
	ktrace("EXIT");
	process_exit(p, (int)exit_code);
}
