#include <stdint.h>
#include <stdio.h>

#include <syscall_func.h>

char sbrk_buf[4096];
char *sbrk_cur = sbrk_buf;

void *sbrk(intptr_t __delta) {
	char *old = sbrk_cur;
	sbrk_cur += __delta;
	return old;
}

void _exit(int __status) {
	__shutdown();
	//__exit(__status);
}
