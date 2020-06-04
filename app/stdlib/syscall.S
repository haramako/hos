#include "syscall.h"
	
	.intel_syntax noprefix

#define MAKE_SYSCALL(syscall_idx, name) .global name ; name: ; mov rax, syscall_idx; syscall; ret

MAKE_SYSCALL(SYS_WRITE, _sys_write)
MAKE_SYSCALL(SYS_EXIT, _sys_exit)
MAKE_SYSCALL(SYS_SHUTDOWN, _sys_shutdown)
MAKE_SYSCALL(SYS_OPEN, _sys_open)
MAKE_SYSCALL(SYS_READ, _sys_read)
