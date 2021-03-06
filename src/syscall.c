#include "syscall.h"

#include "include/syscall.h"

#include "console.h"
#include "gdt.h"
#include "process.h"
#include "scheduler.h"

typedef uint64_t (*SyscallHandlerFunc)(uint64_t *args);

SyscallHandlerFunc syscall_handlers_[SYS_MAX];

uint64_t syscall_noation(uint64_t *args);
uint64_t syscall_chdir(uint64_t *args);
uint64_t syscall_close(uint64_t *args);
uint64_t syscall_creat(uint64_t *args);
uint64_t syscall_dup(uint64_t *args);
uint64_t syscall_exit(uint64_t *args);
uint64_t syscall_fstat(uint64_t *args);
uint64_t syscall_lseek(uint64_t *args);
uint64_t syscall_mkdir(uint64_t *args);
uint64_t syscall_open(uint64_t *args);
uint64_t syscall_read(uint64_t *args);
uint64_t syscall_rename(uint64_t *args);
uint64_t syscall_rmdir(uint64_t *args);
uint64_t syscall_stat(uint64_t *args);
uint64_t syscall_time(uint64_t *args);
uint64_t syscall_write(uint64_t *args);

uint64_t syscall_brk(uint64_t *args);
uint64_t syscall_getdents(uint64_t *args);

uint64_t syscall_shutdown(uint64_t *args);

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

	syscall_handlers_[SYS_EXIT] = syscall_exit;
	syscall_handlers_[SYS_WRITE] = syscall_write;
	syscall_handlers_[SYS_SHUTDOWN] = syscall_shutdown;
	syscall_handlers_[SYS_OPEN] = syscall_open;
	syscall_handlers_[SYS_READ] = syscall_read;
}

__attribute__((ms_abi)) uint64_t SyscallHandler(uint64_t *args) {
	uint64_t idx = args[0];

	// klog("syscall %lld, %lld, %lld, %lld, %lld", args[0], args[1], args[2], args[3], args[4]);

	if (idx > SYS_MAX) {
		kpanic("Unknown syscall!");
	}

	SyscallHandlerFunc handler = syscall_handlers_[idx];
	if (!handler) {
		kpanic("Unknown syscall!");
	}
	return handler(args);
}
