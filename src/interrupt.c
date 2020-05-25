#include "interrupt.h"

#include "boot_param.h"

#define INTERRUPT_LEN 0x100

IDTGateDescriptor descriptors_[INTERRUPT_LEN];
InterruptHandler handler_list_[INTERRUPT_LEN];

/**
 * Interrupt handler.
 *
 * called from AsmIntHandler* in asm.S.
 */
__attribute__((ms_abi)) void IntHandler(uint64_t intcode, InterruptInfo *info) {
	// int_handler_(intcode, info);
	if (intcode <= 0xFF && handler_list_[intcode]) {
		handler_list_[intcode](intcode, info);
	} else {
		kinfo("Int handler not implemented intcode=%d", intcode);
	}
}

void set_entry_(int index, uint8_t segm_desc, uint8_t ist, IDTType type, uint8_t dpl,
				__attribute__((ms_abi)) void (*handler)()) {
	IDTGateDescriptor *desc = &descriptors_[index];
	desc->segment_descriptor = segm_desc;
	desc->interrupt_stack_table = ist;
	desc->type = (int)type;
	desc->descriptor_privilege_level = dpl;
	desc->present = 1;
	desc->offset_low = (uint64_t)handler & 0xffff;
	desc->offset_mid = ((uint64_t)handler >> 16) & 0xffff;
	desc->offset_high = ((uint64_t)handler >> 32) & 0xffffffff;
	desc->reserved0 = 0;
	desc->reserved1 = 0;
	desc->reserved2 = 0;
}

static void handler_general_protection_fault_(uint64_t intcode, InterruptInfo *info) {
	klog("GPF at %016p", info->int_ctx.rip);
	for (;;)
		;
}

static void handler_undefined_operation_(uint64_t intcode, InterruptInfo *info) {
	klog("Undefined Operation Exception at %016p", info->int_ctx.rip);
	for (;;)
		;
}

void interrupt_init() {
	uint16_t cs = ReadCSSelector();

	IDTR idtr;
	idtr.limit = sizeof(descriptors_) - 1;
	idtr.base = descriptors_;

	for (int i = 0; i < INTERRUPT_LEN; i++) {
		set_entry_(i, cs, 1, kInterruptGate, 0, AsmIntHandlerNotImplemented);
		handler_list_[i] = NULL;
	}

	set_entry_(0x00, cs, 0, kInterruptGate, 0, AsmIntHandler00_DivideError);
	set_entry_(0x03, cs, 0, kInterruptGate, 0, AsmIntHandler03);
	set_entry_(0x06, cs, 0, kInterruptGate, 0, AsmIntHandler06);
	set_entry_(0x07, cs, 0, kInterruptGate, 0, AsmIntHandler07_DeviceNotAvailable);
	set_entry_(0x08, cs, 1, kInterruptGate, 0, AsmIntHandler08);
	set_entry_(0x0d, cs, 0, kInterruptGate, 0, AsmIntHandler0D);
	set_entry_(0x0e, cs, 1, kInterruptGate, 0, AsmIntHandler0E);
	set_entry_(0x10, cs, 0, kInterruptGate, 0, AsmIntHandler10_x87FPUError);
	set_entry_(0x13, cs, 0, kInterruptGate, 0, AsmIntHandler13_SIMDFPException);
	set_entry_(0x20, cs, 0, kInterruptGate, 0, AsmIntHandler20);
	set_entry_(0x21, cs, 0, kInterruptGate, 0, AsmIntHandler21);
	set_entry_(0x28, cs, 0, kInterruptGate, 0, AsmIntHandler28);

	WriteIDTR(&idtr);

	interrupt_set_int_handler(0x0d, handler_general_protection_fault_);
	interrupt_set_int_handler(0x06, handler_undefined_operation_);
}

void interrupt_set_int_handler(uint64_t intcode, InterruptHandler handler) {
	assert(intcode < INTERRUPT_LEN);
	handler_list_[intcode] = handler;
}
