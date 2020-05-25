.intel_syntax noprefix

// Microsoft x64 calling convention:
//   args: rcx, rdx, r8, r9
//   callee-saved: RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15
//   caller-saved: RAX, RCX, RDX, R8, R9, R10, R11
// System V AMD64 ABI:
//   args: RDI, RSI, RDX, RCX, R8, R9
//   callee-saved: RBX, RBP, R12, R13, R14, R15
//   caller-saved: otherwise
// Linux kernel x86-64 syscall:
//   idx: RAX
//   args: RDI, RSI, RDX, R10, R8, R9

// Following functions are written with ms_abi calling convention.
// __attribute__((ms_abi)) will be required when used from C code.

/* This macro avoids symbol mismatching in test binary on macOS */
#if defined(__APPLE__)
# define cdecl(s) _##s
#else
# define cdecl(s) s
#endif

.global asm_read_cr3
asm_read_cr3:
	mov rax, cr3
	ret
	
.global asm_write_cr3
asm_write_cr3:	
	mov cr3, rcx
	ret

.global asm_jump_to_kernel
asm_jump_to_kernel:
	// rcx: kernel ptr
	// rdx: kernel info
	// r8: kernel stack
	// convert calling convention from Microsoft x64 to SystemV AMD64
	push rbp
	mov rbp, rsp
	//mov rsp, r8

	mov rdi, rdx
	call rcx

	mov rsp, rbp
	pop rbp
	ret

