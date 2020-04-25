#include "stdint.h"
#include "asm.h"

typedef struct LiumOS {
	int n;
} LiumOS;

uint16_t port_;

void serial_init(uint16_t port) {
  // https://wiki.osdev.org/Serial_Ports
  port_ = port;
  WriteIOPort8(port_ + 1, 0x00);  // Disable all interrupts
  WriteIOPort8(port_ + 3, 0x80);  // Enable DLAB (set baud rate divisor)
  const uint16_t baud_divisor = 0x0001;  // baud rate = (115200 / baud_divisor)
  WriteIOPort8(port_ + 0, baud_divisor & 0xff);
  WriteIOPort8(port_ + 1, baud_divisor >> 8);
  WriteIOPort8(port_ + 3, 0x03);  // 8 bits, no parity, one stop bit
  WriteIOPort8(port_ + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
  WriteIOPort8(port_ + 4, 0x0B);  // IRQs enabled, RTS/DSR set
}

int KernelEntry(LiumOS* liumos_passed)
{
	while(1);
	
	return 0;
}
