default: sh.elf

include ../../common.mk

TARGET = sh.elf
SRCS = $(wildcard *.c *.S) 

CFLAGS= -O2 -std=c11 -target x86_64-unknown-none-elf -fno-builtin \
  -Wall -Wpedantic -Wextra -Wno-keyword-macro -Wno-unused-parameter -Wno-gnu-include-next -Wno-unused-variable \
  -I. -I../../src/include -I../../vendor/newlib/include -I../../cstd/include \
  -D__ELF__ -D_LDBL_EQ_DBL -DUSE_INCLUDE_NEXT -D_POSIX_C_SOURCE=200112 \
  -mcmodel=large

LDFLAGS=-nostdlib -e _start -L../../vendor/newlib/lib -L../../cstd/src -lc -lstd -lc -static

$(TARGET): $(SRCS) Makefile ../app.ld
	$(LLVM_CC) $(CFLAGS) -o $@ -T ../app.ld $(SRCS) $(LDFLAGS)

clean :
	-rm -rf $(TARGET)
