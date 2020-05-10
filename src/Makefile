default: LIUMOS.ELF

include ../common.mk

SRCS= \
	ahci.c \
	apic.c \
	asm.S \
	console.c \
	common_k.c \
	efi.c \
	execution_context.c \
	fat.c \
	gdt.c \
	hpet.c \
	interrupt.c \
	interrupt_handler.c \
	main.c \
	mem.c \
	newlib_support.c \
	page.h \
	pci.h \
	physical_memory.c \
	process.c \
	serial.c \
	scheduler.c \
	sleep_handler.S \
	syscall.c \
	syscall_handler.S \
	timer.c 

OBJS= $(addsuffix .o, $(basename $(SRCS)))
DEPS= $(addsuffix .d, $(basename $(SRCS)))

CLANG_WARNINGS = \
	-Wall -Wpedantic -Wextra -Wconditional-uninitialized -Wshorten-64-to-32 \
	-Werror \
	-Wno-keyword-macro \
	-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function \
	-Wno-char-subscripts \
    -Wno-gnu-binary-literal

NEWLIB_INC_PATH=../third_party_root/include
NEWLIB_LIB_PATH=../third_party_root/lib

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

%.o : %.c Makefile
	$(LLVM_CC) $(CFLAGS) -MD -g -c $*.c

%.o : %.S Makefile
	$(LLVM_CC) $(CFLAGS) -c -o $*.o $*.S

.PHONY: bootfs.o
bootfs.o:
	make -C .. bootfs

LIUMOS.ELF : $(OBJS) kernel.ld Makefile bootfs.o
	$(LLVM_LD_LLD) $(LDFLAGS) -o LIUMOS.ELF -e kernel_entry -T kernel.ld $(OBJS) bootfs.o

run:
	make -C .. run

runc:
	make -C .. runc

clean :
	-rm -rf *.EFI
	-rm -rf *.o
	-rm -rf *.d

format :
	clang-format -i *.c *.h
	-gtags

-include $(DEPS)
