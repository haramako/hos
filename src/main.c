#include "liumos.h"

#include "asm.h"
#include "gdt.h"
#include "hpet.h"
#include "interrupt.h"
#include "mem.h"
#include "physical_memory.h"
#include "scheduler.h"
#include "serial.h"
#include "timer.h"

LiumOS *g_liumos;

void kernel_entry(LiumOS *liumos_passed);

static void test_malloc_() {
	int *a = (int *)malloc(4);
	int *b = (int *)malloc(4);
	free(a);
	int *c = (int *)malloc(4);
	int n = 99;

	klog("a = %p\nb= %p\nc= %p", (void *)a, (void *)b, c);

	klog("KernelEntry %016llx", kernel_entry);
	klog("g_liumos     %016llx", g_liumos);
	klog("n   _       %016llx", &n);
}

static void test_virtual_memory_map_() {
	EFI_RuntimeServices *runtime_services = g_liumos->loader_info.efi->system_table->runtime_services;
	EFI_MemoryMap *mm = g_liumos->efi_memory_map;
	int res = runtime_services->set_virtual_address_map(sizeof(mm->buf), mm->descriptor_size, mm->key, (void *)mm->buf);
	klog("res = %d", res);
	efi_memory_map_print(g_liumos->efi_memory_map);
}

static void test_memory_map_() { efi_memory_map_print(g_liumos->efi_memory_map); }

static void test_reset_() {
	EFI_RuntimeServices *runtime_services = g_liumos->loader_info.efi->system_table->runtime_services;
	runtime_services->reset_system(EfiResetShutdown, 0, 0, NULL);
}

// Timer test
static void timer_test_callback1_(TimerParam *p, void *data) { klog("call_periodic"); }

static void timer_test_callback2_(TimerParam *p, void *data) { klog("call_after"); }

static void timer_test_() {
	timer_call_periodic(1 * SEC, timer_test_callback1_, NULL);
	timer_call_after(3 * SEC, timer_test_callback2_, NULL);
	timer_print();
	for (;;) {
		hpet_busy_wait(1000);
		console_write(".");
	}
}

// Process test
void process_switch_context(InterruptInfo *int_info, Process *from_proc, Process *to_proc) {
#if 0
	static uint64_t proc_last_time_count = 0;
	const uint64_t now_count = time_now();
	if ((proc_last_time_count - now_count) < liumos->time_slice_count)
		return;

	from_proc.AddProcTimeFemtoSec(
								  (liumos->hpet->ReadMainCounterValue() - proc_last_time_count) *
								  liumos->hpet->GetFemtosecondPerCount());
#endif

	// process_print(from_proc);
	// process_print(to_proc);
	CPUContext *from = &from_proc->ctx->cpu_context;
	const uint64_t t0 = time_now();
	from->cr3 = ReadCR3();
	from->greg = int_info->greg;
	from->int_ctx = int_info->int_ctx;
	process_notify_contextsaving(from_proc);
	const uint64_t t1 = time_now();
	from_proc->time_consumed_in_ctx_save_femto_sec += t1 - t0;

	CPUContext *to = &to_proc->ctx->cpu_context;
	int_info->greg = to->greg;
	int_info->int_ctx = to->int_ctx;
	if (from->cr3 != to->cr3) {
		WriteCR3(to->cr3);
		// proc_last_time_count = liumos->hpet->ReadMainCounterValue();
	}
	// klog("3 %p", int_info->int_ctx.rip);
}

#if 0
__attribute__((ms_abi)) void SleepHandler(uint64_t intcode, InterruptInfo *info) {
	assert(info);
	Process *proc = g_scheduler.current;
	Process *next_proc = scheduler_switch_process();
	if (!next_proc) return; // no need to switching context.
	process_switch_context(info, proc, next_proc);
}
#else
__attribute__((ms_abi)) void SleepHandler(uint64_t intcode, InterruptInfo *info) {}
#endif

// Process test
static void process_test_timer_(uint64_t intcode, InterruptInfo *info) {
	apic_send_end_of_interrupt(&g_apic);
	// klog("process_test_timer_");
	assert(info);
	Process *proc = g_scheduler.current;
	Process *next_proc = scheduler_switch_process();
	if (!next_proc) return; // no need to switching context.
	process_switch_context(info, proc, next_proc);
}

static void process_test_process1_() {
	klog("[1]");
	for (;;) {
		hpet_busy_wait(1000);
		console_write("1");
	}
}

static void process_test_process2_() {
	klog("[2]");
	for (;;) {
		hpet_busy_wait(1000);
		console_write("2");
	}
}

static void process_test_() {
	// timer_call_periodic(1 * MSEC, , NULL);

	{
		ExecutionContext *ctx = malloc(sizeof(ExecutionContext));
		char *sp = malloc(1024 * 4);
		char *kernel_sp = malloc(1024 * 4);
		uint64_t cr3 = ReadCR3();
		klog("sp %p", sp);
		execution_context_new(ctx, process_test_process1_, sp + 1024 * 4, cr3, kRFlagsInterruptEnable,
							  (uint64_t)(kernel_sp + 1024 * 4));

		Process *p = malloc(sizeof(Process));
		process_new(p, ctx);

		scheduler_register_process(p);
	}

	{
		ExecutionContext *ctx = malloc(sizeof(ExecutionContext));
		char *sp = malloc(1024 * 4);
		char *kernel_sp = malloc(1024 * 4);
		uint64_t cr3 = ReadCR3();
		execution_context_new(ctx, process_test_process2_, sp + 1024 * 4, cr3, kRFlagsInterruptEnable,
							  (uint64_t)(kernel_sp + 1024 * 4));

		Process *p = malloc(sizeof(Process));
		process_new(p, ctx);

		scheduler_register_process(p);
	}

	// process_switch_context();

	interrupt_set_int_handler(0x28, process_test_timer_);
	hpet_set_timer_ns(1, 100 * MSEC, HPET_TC_ENABLE | HPET_TC_USE_PERIODIC_MODE);

	//__asm__("int $0x28");

	for (;;) {
		hpet_busy_wait(1000);
		console_write(".");
	}
}

void kernel_entry(LiumOS *liumos_passed) {
	g_liumos = liumos_passed;

	Disable8259PIC();

	serial_init();
	console_init(serial_get_port(1));
	console_set_log_level(CONSOLE_LOG_LEVEL_TRACE);

	// Now you can use console_*().

	console_write("\n");
	kinfo("=======================");
	kinfo("Kernel Start");

	physical_memory_init(g_liumos->efi_memory_map);
	mem_init();

	// Now you can use malloc/free.

	apic_init();

	const int stack_pages = 1024 * 16;
	uintptr_t stack = physical_memory_alloc(stack_pages);
	uintptr_t ist = physical_memory_alloc(stack_pages);
	gdt_init(stack + stack_pages * PAGE_SIZE, ist + stack_pages * PAGE_SIZE);
	interrupt_init();

	// Now ready to interrupt (but interrupt flag is not set).

	hpet_init((HPET_RegisterSpace *)g_liumos->acpi.hpet->base_address.address);
	timer_init();

	scheduler_init();

	// Now ready to HPET timer.

	StoreIntFlag(); // Start interrupt.

	// =========================

	kinfo("Kernel Ready!");
	kinfo("Time %lld", hpet_read_main_counter_value());

	process_test_();

	Die();
}
