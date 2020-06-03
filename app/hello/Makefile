default: hello.elf

include ../../common.mk

TARGET = hello.elf
SRCS = hello.c syscall.S

CFLAGS= -std=c11 -target x86_64-unknown-none-elf -nostdlib -fno-builtin \
  -Wall -Wpedantic -Wextra -Wno-keyword-macro \
  -I../../src/include -I../../vendor/newlib/include -D__ELF__ -D_LDBL_EQ_DBL \
  -mcmodel=large


#LDFLAGS=-L../../third_party_root/lib -lc -static
LDFLAGS=-e _start

# Kernel rules

$(TARGET): $(SRCS) Makefile
	$(LLVM_CC) $(CFLAGS) -o $@ -T kernel.ld $(SRCS) $(LDFLAGS)

clean :
	-rm -rf $(TARGET)

