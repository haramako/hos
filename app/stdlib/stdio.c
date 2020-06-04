#include <stdio.h>

int _sys_write(int fd, const void *, size_t);
int _sys_open(const char *path, int flag);

int open (const char *__file, int __oflag, ...){
	return _sys_open(__file, __oflag);
}

_READ_WRITE_RETURN_TYPE write (int __fd, const void *__buf, size_t __nbyte){
	_sys_write(1, __buf, __nbyte);
	return __nbyte;
}

_READ_WRITE_RETURN_TYPE read (int __fd, void *__buf, size_t __nbytes){
	return 0;
}
