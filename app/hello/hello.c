typedef long long size_t;

int _sys_write(int fd, const void *, size_t);
void _sys_exit(int);
void _sys_shutdown();

int _sys_open(const char *path, int flag);
int _sys_read(int fd, void *buf, size_t size);

void _start(){
	_sys_write(1, "Hello, world!\n", 14);
	_sys_shutdown();

	for(;;);
}
