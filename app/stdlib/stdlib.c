#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int _sys_write(int fd, const void *, size_t);
void _sys_exit(int);
void _sys_shutdown();

int _sys_open(const char *path, int flag);
int _sys_read(int fd, void *buf, size_t size);

void _start(){

	char buf[16];
	FILE *fd = fopen("hoge.txt", "w" );
	snprintf(buf, sizeof(buf), "%d\n", 99);
	int err = fwrite(buf, 1, strlen(buf), fd);
	fflush(fd);
	printf("err %d\n", err);

	char *x1 = malloc(8);
	char *x2 = malloc(8);
	printf("hoge %p %p\n", (void*)x1, (void*)x2);

	#if 0
	int len = _sys_read(fd, buf, 8);

	if( len > 0 ){
		_sys_write(1, buf, len);
	}

	//int n = strlen(buf);

	snprintf(buf, sizeof(buf), "%s %d\n", "STR", 1);
	_sys_write(1, buf, strlen(buf));
	
	_sys_exit(0);
	#endif

	_sys_shutdown();

	for(;;);
}

//caddr_t sbrk(int diff) { _sys_shutdown(); return 0; }


