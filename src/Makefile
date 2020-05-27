default: KERNEL.ELF

include ../common.mk

SRCS= \
	acpi.c \
	ahci.c \
	apic.c \
	asm.S \
	asm_inline.c \
	console.c \
	common_k.c \
	efi.c \
	execution_context.c \
	font.gen.c \
	fat.c \
	gdt.c \
	hpet.c \
	interrupt.c \
	interrupt_handler.S \
	main.c \
	mem.c \
	mm.c \
	mutex.c \
	newlib_support.c \
	page.c \
	page_print.c \
	pagefault.c \
	pci.c \
	physical_memory.c \
	process.c \
	serial.c \
	scheduler.c \
	sheet.c \
	sheet_painter.c \
	sleep_handler.S \
	smp.c \
	smp_trampoline.S \
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
	-g \
	-O2 \
	-std=c11 \
	-target x86_64-unknown-none-elf \
	-fno-stack-protector -fno-exceptions -fshort-wchar \
	-mno-red-zone -mcmodel=large \
	-nostdlib \
	-D__ELF__ -D_LDBL_EQ_DBL \
	-I. \
	-I../boot/include \
	-I$(NEWLIB_INC_PATH) \
	$(CLANG_WARNINGS) $(C_STD)

LDFLAGS= \
	-L$(NEWLIB_LIB_PATH) -lm \
	-L../third_party_root/lib -lc \
	-static # --verbose

# Kernel rules

%.o : %.c Makefile
	$(LLVM_CC) $(CFLAGS) -MD -c $*.c

%.o : %.S Makefile
	$(LLVM_CC) $(CFLAGS) -c -o $*.o $*.S

KERNEL.ELF : $(OBJS) kernel.ld Makefile
	$(LLVM_LD_LLD) $(LDFLAGS) -o KERNEL.ELF -e kernel_entry -T kernel.ld $(OBJS)

run:
	make -C .. run

runc:
	make -C .. runc

clean :
	-rm -rf *.EFI
	-rm -rf *.o
	-rm -rf *.d

depgraph:
	rexdep --format dot --pattern '^\s*#include\s*"(\S+)"' *.h | grep -v 'common.h' | dot -Tpng -o deps_header.png
	rexdep --format dot --pattern '^\s*#include\s*"(\S+)"' *.h | dot -Tpng -o deps_header_all.png
	rexdep --format dot --pattern '^\s*#include\s*"(\S+)"' *.[ch] | grep -vE 'common.h|main.c|asm' | sed -E 's/\.(h|c)//g' | dot -Tpng -o deps_all.png

-include $(DEPS)
