default: hello.elf

include ../../common.mk

TARGET = hello.elf
SRCS = hello.c syscall.S

CFLAGS= -std=c11 -target x86_64-unknown-none-elf -nostdlib -Wall -Wpedantic -Wextra -Werror

LDFLAGS=-static

# Kernel rules

$(TARGET): $(SRCS) Makefile
	$(LLVM_CC) $(CFLAGS) -o $@ -T kernel.ld $(SRCS)

clean :
	-rm -rf $(TARGET)

