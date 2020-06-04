#pragma once

#include <stdint.h>

int __write(int fd, const void *, size_t);
void __exit(int);
void __shutdown();

int __open(const char *path, int flag);
int __read(int fd, void *buf, size_t size);
