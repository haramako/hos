#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <syscall_func.h>

char sbrk_buf[4096*4];
char *sbrk_cur = sbrk_buf;

int main(int argc, char **argv);

void _start(){
	int exitcode = main(0, NULL);
	__shutdown();
	exit(exitcode);
}

void *sbrk(intptr_t __delta) {
	char *old = sbrk_cur;
	sbrk_cur += __delta;
	return old;
}

void _exit(int __status) {
	__shutdown();
	//__exit(__status);
}
