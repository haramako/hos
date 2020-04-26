#include <stddef.h>
#include <sys/types.h>

#include "mem.h"

caddr_t sbrk(int diff)
{
	return (caddr_t)mem_sbrk(diff);
}

static void Panic(const char* msg)
{
}

void _exit(int code) {
  Panic("_exit");
}

void kill(int pid) {
  Panic("kill");
}

void getpid(int pid) {
  Panic("getpid");
}

void close(int fd) {
  Panic("close");
}

void lseek(int fd) {
  Panic("lseek");
}

void read(int fd) {
  Panic("lseek");
}

void write(int fd) {
  Panic("write");
}
void fstat(int fd) {
  Panic("fstat");
}
void isatty(int fd) {
  Panic("isatty");
}
