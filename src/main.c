#include "liumos.h"

#include "asm.h"
#include "gdt.h"
#include "hpet.h"
#include "interrupt.h"
#include "mem.h"
#include "page.h"
#include "physical_memory.h"
#include "scheduler.h"
#include "serial.h"
#include "timer.h"

#include "main_test.c" // Include test source.

LiumOS *g_liumos;
static void paging_test_() {
	uint64_t cr0 = asm_read_cr0();
	uint64_t cr3 = ReadCR3();
	klog("CR0 %p", cr0);
	klog("CR3 %p", cr3);

	// int n = *((int*)0x01);
	//*((int*)0x01) = 1;

	int *x = (int *)0x0000000080000000;
	// int * x = (int*)0x000000007f000000;
	// int * x = (int*)0x000000007fffffff;
	// int * x = (int*)0x00000000ffe00000;
	klog("%d", *x);
	*x = 1;
	klog("%d", *x);

	PageMapEntry *pml4 = (PageMapEntry *)cr3;

	void *p = (void *)(7 * 1024 * 1024 + 123);
	klog("v2p %p => %p", p, page_v2p(pml4, p));

	pme_print(pml4);

	PageMapEntry *new_pml4 = page_copy_page_map_table(pml4);

	klog("=================");
	pme_print(new_pml4);

	{
		int *x2 = (int *)0xffff800000000100;
		page_alloc_addr((void *)((uint64_t)x2 & ~(PAGE_SIZE - 1)), 100, false);
		*x2 = 1;

		klog("=================");
		pme_print(pml4);
	}

	{
		int *x2 = (int *)0xffff800000010000;
		*x2 = 1;

		klog("=================");
		pme_print(pml4);
	}
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

static void init_syscall_() {
	uint64_t star = ((uint64_t)kKernelCSSelector) << 32;
	star |= ((uint64_t)kUserCS32Selector) << 48;
	WriteMSR(MSRIndex_kSTAR, star);

	WriteMSR(MSRIndex_kLSTAR, (uint64_t)AsmSyscallHandler);
	WriteMSR(MSRIndex_kFMASK, 1ULL);

	IA32_EFER efer = {.data = ReadMSR(MSRIndex_kEFER)};
	efer.bits.syscall_enable = 1;
	klog("EFER %p", efer.data);
	WriteMSR(MSRIndex_kEFER, efer.data);
}

static void init_gdt_and_interrupt_() {
	const int stack_pages = 1024 * 16;
	uintptr_t stack = physical_memory_alloc(stack_pages);
	uintptr_t ist = physical_memory_alloc(stack_pages);
	gdt_init(stack + stack_pages * PAGE_SIZE, ist + stack_pages * PAGE_SIZE);
	interrupt_init();
	page_init_interrupt();
}

static void init_process_and_scheduler_() {
	scheduler_init();

	interrupt_set_int_handler(0x28, process_timer_handler);
	hpet_set_timer_ns(1, 100 * MSEC, HPET_TC_ENABLE | HPET_TC_USE_PERIODIC_MODE);
}

void kernel_entry(LiumOS *liumos_passed) {
	g_liumos = liumos_passed;

	Disable8259PIC();

	serial_init();
	console_init(serial_get_port(1));
	console_set_log_level(CONSOLE_LOG_LEVEL_INFO);

	// Now you can use console_*().

	console_write("\n");
	kinfo("=======================");
	kinfo("Kernel Start");

	init_syscall_();
	physical_memory_init(g_liumos->efi_memory_map);
	page_init();
	mem_init();

	// Now you can use malloc/free.

	apic_init();
	init_gdt_and_interrupt_();

	// Now ready to interrupt (but interrupt flag is not set).

	hpet_init((HPET_RegisterSpace *)g_liumos->acpi.hpet->base_address.address);
	timer_init();

	init_process_and_scheduler_();

	// Now ready to HPET timer.

	StoreIntFlag(); // Start interrupt.

	// =========================

	kinfo("Kernel Ready!");
	kinfo("Time %lld", hpet_read_main_counter_value());

	console_set_log_level(CONSOLE_LOG_LEVEL_TRACE);

	// test_virtual_memory_map_();
	// paging_test_();
	// process_test_();
	process_test2_();

	Die();
}
