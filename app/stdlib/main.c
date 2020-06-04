#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "syscall_func.h"

void _start(){

	char buf[16];
	FILE *fd = fopen("hoge.txt", "r" );

	int len = fread(buf, 1, 4, fd);
	buf[len] = '\0';

	printf("read, len=%d, buf=%s\n", len, buf);

	char *x1 = malloc(8);
	char *x2 = malloc(8);
	printf("malloc %p %p\n", (void*)x1, (void*)x2);

	// exit(0);

	__shutdown();

	for(;;);
}


