#include "pipe.h"

#include <errno.h>

Pipe *pipe_new() {
	Pipe *p = talloc(Pipe);
	return p;
}

error_t pipe_read(Pipe *p, void *buf_, size_t len) {
	uint8_t *buf = (uint8_t *)buf_;
	size_t used = pipe_used(p);
	if (len > used) {
		return ENOBUFS;
	}

	while (len > 0) {
		size_t size = MIN(len, PIPE_BUF_LEN - p->head);
		memcpy(buf, p->buf + p->head, size);
		p->head = (p->head + size) % PIPE_BUF_LEN;
		buf += size;
		len -= size;
	}

	return ERR_OK;
}

error_t pipe_write(Pipe *p, const void *buf_, size_t len) {
	uint8_t *buf = (uint8_t *)buf_;
	size_t rest = pipe_rest(p);
	if (len > rest) {
		return ENOBUFS;
	}

	while (len > 0) {
		size_t size = MIN(len, PIPE_BUF_LEN - p->tail);
		memcpy(p->buf + p->tail, buf, size);
		p->tail = (p->tail + size) % PIPE_BUF_LEN;
		buf += size;
		len -= size;
	}

	return ERR_OK;
}

size_t pipe_used(Pipe *p) {
	if (p->head <= p->tail) {
		return p->tail - p->head;
	} else {
		return p->head + PIPE_BUF_LEN - p->tail;
	}
}

size_t pipe_rest(Pipe *p) { return PIPE_BUF_LEN - pipe_used(p); }
