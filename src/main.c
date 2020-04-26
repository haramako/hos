#include "liumos.h"
#include "asm.h"
#include "serial.h"

static Serial com1_;

void KernelEntry(LiumOS* liumos_passed)
{
	serial_init(&com1_, 0x2f8);

	char buf[256];
	snprintf(buf, sizeof(buf), "hoge %d\nFUGA", 1);

	for(int i=0; buf[i] != '\0'; i++){
		serial_send_char(&com1_, buf[i]);
	}

	while(1);
		
	while(1){
		serial_send_char(&com1_, 'a');
	}
}
