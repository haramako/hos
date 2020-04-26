#include <stddef.h>
#include <sys/types.h>

//#include "generic.h"
//#include "liumos.h"

caddr_t sbrk(int diff) {
	#if 0
  Process& proc = liumos->scheduler->GetCurrentProcess();
  ExecutionContext& ctx = proc.GetExecutionContext();
  ctx.ExpandHeap(diff);
  return (caddr_t)ctx.GetHeapEndVirtAddr();
  #else
  return (caddr_t)0x30;
  #endif
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
