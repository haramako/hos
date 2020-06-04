default: stdlib.elf

include ../../common.mk

TARGET = stdlib.elf
SRCS = $(wildcard *.c *.S) 

CFLAGS= -O2 -std=c11 -target x86_64-unknown-none-elf -fno-builtin \
  -Wall -Wpedantic -Wextra -Wno-keyword-macro -Wno-unused-parameter -Wno-gnu-include-next \
  -I../../src/include -I../../vendor/newlib/include -I../../../newlib_posix/include \
  -D__ELF__ -D_LDBL_EQ_DBL -DUSE_INCLUDE_NEXT \
  -mcmodel=large


LDFLAGS=-nostdlib -e _start -L../../vendor/newlib/lib -lc -static

# Kernel rules

$(TARGET): $(SRCS) Makefile kernel.ld
	$(LLVM_CC) $(CFLAGS) -o $@ -T kernel.ld $(SRCS) $(LDFLAGS)

clean :
	-rm -rf $(TARGET)

