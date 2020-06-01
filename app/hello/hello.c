#include <stdint.h>
#include <stdio.h>
#include <string.h>

int _sys_write(int fd, const void *, size_t);
void _sys_exit(int);
void _sys_shutdown();

int _sys_open(const char *path, int flag);
int _sys_read(int fd, void *buf, size_t size);

int n;

void print_int(uint64_t n);


void print_int(uint64_t n){
	char buf[16];
	buf[0] = '0' + n;
	buf[1] = '\n';
	_sys_write(1, buf, 2);
}

void _start(){
	_sys_write(1, "Hello, world!\n", 14);
	n++;
	
	//_sys_exit(0);
	//write(1, "exited!\n", 8);
	//_sys_shutdown();

	int fd = _sys_open("hoge.txt", 0);
	print_int(fd);
	
	char buf[16];
	int len = _sys_read(fd, buf, 8);
	print_int(len);

	if( len > 0 ){
		_sys_write(1, buf, len);
	}
	
	for(;;);
}

//caddr_t sbrk(int diff) { _sys_shutdown(); return 0; }

