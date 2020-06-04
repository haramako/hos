default: libstd.a

include ../../common.mk

TARGET = libstd.a
SRCS = $(wildcard *.c *.S) 

CFLAGS= -O2 -std=c11 -target x86_64-unknown-none-elf -fno-builtin \
  -Wall -Wpedantic -Wextra -Wno-keyword-macro -Wno-unused-parameter -Wno-gnu-include-next \
  -I. -I../../src/include -I../../vendor/newlib/include -I../../cstd/include \
  -D__ELF__ -D_LDBL_EQ_DBL -DUSE_INCLUDE_NEXT \
  -mcmodel=large

$(TARGET): $(SRCS) Makefile
	$(LLVM_CC) $(CFLAGS) -c $(SRCS)
	$(LLVM_AR) -crsv $@ *.o 

clean :
	-rm -rf *.o $(TARGET)

