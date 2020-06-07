#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syscall_func.h"

int main(int argc, char **argv) {

	char buf[16];
	FILE *fd = fopen("hoge.txt", "r");

	int len = fread(buf, 1, 4, fd);
	buf[len] = '\0';

	printf("read, len=%d, buf=%s\n", len, buf);

	char *x1 = malloc(8);
	char *x2 = malloc(8);
	printf("malloc %p %p\n", (void *)x1, (void *)x2);

	return 0;
}
