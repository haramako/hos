default: LIUMOS.ELF

include ../common.mk

SRCS= \
	apic.c \
	asm.S \
	console.c \
	common_k.c \
	efi.c \
	execution_context.c \
	gdt.c \
	hpet.c \
	interrupt.c \
	interrupt_handler.c \
	main.c \
	mem.c \
	newlib_support.c \
	newlib_func.c \
	page.h \
	physical_memory.c \
	process.c \
	serial.c \
	scheduler.c \
	sleep_handler.S \
	syscall_handler.S \
	timer.c 

OBJS= $(addsuffix .o, $(basename $(SRCS)))
DEPS= $(addsuffix .d, $(basename $(SRCS)))

CLANG_WARNINGS = \
	-Wall -Wpedantic -Wextra -Wconditional-uninitialized -Wshorten-64-to-32 \
	-Werror \
	-Wno-keyword-macro \
	-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function \
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

.PHONY: ../app/hello/hello_data.o
../app/hello/hello_data.o:
	make -C ../app/hello

LIUMOS.ELF : $(OBJS) kernel.ld Makefile ../app/hello/hello_data.o
	$(LLVM_LD_LLD) $(LDFLAGS) -o LIUMOS.ELF -e kernel_entry -T kernel.ld $(OBJS) ../app/hello/hello_data.o

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
