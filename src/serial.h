#pragma once

#include "common.h"

typedef struct Serial {
	uint16_t port;
} Serial;

void serial_init(Serial *s, uint16_t port);
bool serial_is_transmit_empty(Serial *s);
void serial_send_char(Serial *s, char c);
