#include "syscall.h"
	
	.intel_syntax noprefix
	.global write
	
write:
	mov rax, SYS_WRITE
	syscall
	ret

.global exit
exit:
	mov rax, SYS_EXIT
	syscall
	ret

.global shutdown
shutdown:
	mov rax, SYS_SHUTDOWN
	syscall
	ret
