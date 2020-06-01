#include <stdio.h>
#include <string.h>

int _sys_write(int fd, const void *, size_t);
void _sys_exit(int);
void _sys_shutdown();

int _sys_open(const char *path, int flag);
int _sys_read(int fd, void *buf, size_t size);

void _start(){
	_sys_write(1, "Hello, world!\n", 14);
	
	//_sys_exit(0);
	//write(1, "exited!\n", 8);
	//_sys_shutdown();

	int fd = _sys_open("HELLO.ELF", 0);
	char buf[16];
	//snprintf(buf, sizeof(buf), "fd %d", fd);
	buf[0] = '0' + fd;
	buf[1] = '\n';
	buf[2] = '\0';
	_sys_write(1, buf, 2);

	int err = _sys_read(fd, buf, 8);
	buf[8] = '\0';

	_sys_write(1, buf, 8);
	
	for(;;);
}

//caddr_t sbrk(int diff) { _sys_shutdown(); return 0; }

