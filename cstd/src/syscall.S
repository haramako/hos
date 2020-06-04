#include "syscall.h"
	
	.intel_syntax noprefix

#define MAKE_SYSCALL(syscall_idx, name) .global name ; name: ; mov rax, syscall_idx; syscall; ret

MAKE_SYSCALL(SYS_WRITE, __write)
MAKE_SYSCALL(SYS_EXIT, __exit)
MAKE_SYSCALL(SYS_SHUTDOWN, __shutdown)
MAKE_SYSCALL(SYS_OPEN, __open)
MAKE_SYSCALL(SYS_READ, __read)
