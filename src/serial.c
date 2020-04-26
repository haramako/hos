#include "serial.h"

#include "asm.h"

void serial_init(Serial *s, uint16_t port)
{
  // https://wiki.osdev.org/Serial_Ports
  s->port = port;
  WriteIOPort8(s->port + 1, 0x00);  // Disable all interrupts
  WriteIOPort8(s->port + 3, 0x80);  // Enable DLAB (set baud rate divisor)
  const uint16_t baud_divisor = 0x0001;  // baud rate = (115200 / baud_divisor)
  WriteIOPort8(s->port + 0, baud_divisor & 0xff);
  WriteIOPort8(s->port + 1, baud_divisor >> 8);
  WriteIOPort8(s->port + 3, 0x03);  // 8 bits, no parity, one stop bit
  WriteIOPort8(s->port + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
  WriteIOPort8(s->port + 4, 0x0B);  // IRQs enabled, RTS/DSR set
}

bool serial_is_transmit_empty(Serial *s) {
	return ReadIOPort8(s->port + 5) & 0x20;
}

void serial_send_char(Serial *s, char c) {
	while (!serial_is_transmit_empty(s));
	WriteIOPort8(s->port, c);
}
