#pragma once

#include "common.h"

#define PIPE_BUF_LEN 4096

typedef struct Pipe_ {
	char buf[PIPE_BUF_LEN];
	size_t head;
	size_t tail;
} Pipe;

Pipe *pipe_new();
error_t pipe_read(Pipe *p, void *buf, size_t len);
error_t pipe_write(Pipe *p, const void *buf, size_t len);
size_t pipe_used(Pipe *p);
size_t pipe_rest(Pipe *p);
