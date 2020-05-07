#include "syscall.h"

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
#if 0
  uint64_t idx = args[0];
  if (idx == kSyscallIndex_sys_write) {
    uint64_t t0 = liumos->hpet->ReadMainCounterValue();
    const uint64_t fildes = args[1];
    const uint8_t* buf = reinterpret_cast<uint8_t*>(args[2]);
    uint64_t nbyte = args[3];
    if (fildes != 1) {
      PutStringAndHex("fildes", fildes);
      Panic("Only stdout is supported for now.");
    }
    while (nbyte--) {
      PutChar(*(buf++));
    }
    uint64_t t1 = liumos->hpet->ReadMainCounterValue();
    liumos->scheduler->GetCurrentProcess().AddSysTimeFemtoSec(
        (t1 - t0) * liumos->hpet->GetFemtosecondPerCount());
    return;
  } else if (idx == kSyscallIndex_sys_exit) {
    const uint64_t exit_code = args[1];
    PutStringAndHex("exit: exit_code", exit_code);
    liumos->scheduler->KillCurrentProcess();
    ExecutionContext& ctx =
        liumos->scheduler->GetCurrentProcess().GetExecutionContext();
    ChangeRSP(ctx.GetKernelRSP());
    for (;;) {
      StoreIntFlagAndHalt();
    };
  } else if (idx == kSyscallIndex_arch_prctl) {
    Panic("arch_prctl!");
    if (args[1] == kArchSetFS) {
      WriteMSR(MSRIndex::kFSBase, args[2]);
      return;
    }
    PutStringAndHex("arg1", args[1]);
    PutStringAndHex("arg2", args[2]);
    PutStringAndHex("arg3", args[3]);
  }
  PutStringAndHex("idx", idx);
  Panic("syscall handler!");
#endif
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
