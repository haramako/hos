// Test code, included directory from main.c.

static void test_malloc_() {
	int *a = (int *)malloc(4);
	int *b = (int *)malloc(4);
	free(a);
	int *c = (int *)malloc(4);
	int n = 99;

	klog("a = %p\nb= %p\nc= %p", (void *)a, (void *)b, c);

	klog("KernelEntry %016llx", hpet_init);
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

Process *process_create(void (*entry)()) {
	ExecutionContext *ctx = malloc(sizeof(ExecutionContext));
	char *sp = malloc(1024 * 4);
	char *kernel_sp = malloc(1024 * 4);
	uint64_t cr3 = ReadCR3();
	execution_context_new(ctx, entry, sp + 1024 * 4, cr3, kRFlagsInterruptEnable, (uint64_t)(kernel_sp + 1024 * 4));

	Process *p = malloc(sizeof(Process));
	process_new(p, ctx);

	scheduler_register_process(p);

	return p;
}

static void process_test_() {
	process_create(process_test_process1_);
	process_create(process_test_process2_);

	for (;;) {
		hpet_busy_wait(1000);
		console_write(".");
	}
}
