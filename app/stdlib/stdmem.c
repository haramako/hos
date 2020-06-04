#include <stdint.h>

char sbrk_buf[1024];
void *sbrk (intptr_t __delta){
	if( __delta == 0){
		return sbrk_buf;
	}else{
		return sbrk_buf + 1024;
	}
}

