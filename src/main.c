#include "liumos.h"
#include "asm.h"
#include "serial.h"

void KernelEntry(LiumOS* liumos_passed)
{
	serial_init(0x2f8);

	char buf[256];
	snprintf(buf, sizeof(buf), "hoge %d\nFUGA", 1);

	for(int i=0; buf[i] != '\0'; i++){
		serial_send_char(buf[i]);
	}

	while(1);
		
	while(1){
		serial_send_char('a');
	}
}
