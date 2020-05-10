default: hello.elf

include ../../common.mk

TARGET = hello.elf
SRCS = hello.c syscall.S

CLANG_WARNINGS = \
	-Wall -Wpedantic -Wextra -Wconditional-uninitialized -Wshorten-64-to-32 \
	-Werror \
	-Wno-keyword-macro \
	-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function \
    -Wno-gnu-binary-literal

NEWLIB_INC_PATH=../../third_party_root/include
NEWLIB_LIB_PATH=../../third_party_root/lib

CFLAGS= \
	-std=c11 \
	-target x86_64-unknown-none-elf \
	-fno-stack-protector -fno-exceptions -fshort-wchar \
	-mno-red-zone -mcmodel=large \
	-nostdlib \
	-D__ELF__ -D_LDBL_EQ_DBL \
	-I$(NEWLIB_INC_PATH) \
	$(CLANG_WARNINGS) $(C_STD)

LDFLAGS= \
	-L$(NEWLIB_LIB_PATH) -lm \
	-L../third_party_root/lib -lc \
	-static # --verbose

# Kernel rules

$(TARGET): $(SRCS) Makefile
	$(LLVM_CC) $(CFLAGS) -o $@ -T kernel.ld $(SRCS)

clean :
	-rm -rf $(TARGET)

