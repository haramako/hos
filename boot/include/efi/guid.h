#pragma once

#include <stdint.h>

typedef struct PACKED GUID_ {
	uint32_t id1;
	uint16_t id2;
	uint16_t id3;
	uint8_t id4[8];
} GUID;
