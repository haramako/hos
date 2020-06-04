default: hello.elf

include ../../common.mk

TARGET = hello.elf
SRCS = hello.c syscall.S

CFLAGS= -O2 -std=c11 -target x86_64-unknown-none-elf -fno-builtin \
  -Wall -Wno-keyword-macro \
  -I../../src/include -I../../vendor/newlib/include -D__ELF__ -D_LDBL_EQ_DBL \
  -mcmodel=large


LDFLAGS=-nostdlib -e _start -L../../vendor/newlib/lib -lc -static

# Kernel rules

$(TARGET): $(SRCS) Makefile kernel.ld
	$(LLVM_CC) $(CFLAGS) --verbose -o $@ -T kernel.ld $(SRCS) $(LDFLAGS)

clean :
	-rm -rf $(TARGET)

