default: LIUMOS.ELF

include ../common.mk

KERNEL_SRCS= \
	apic.c \
	asm.S \
	console.c \
	efi.c \
	gdt.c \
	hpet.c \
	interrupt.c \
	interrupt_handler.c \
	main.c \
	mem.c \
	newlib_support.c \
	newlib_func.c \
	physical_memory.c \
	serial.c \
	sleep_handler.S

KERNEL_OBJS= $(addsuffix .o, $(basename $(KERNEL_SRCS)))
KERNEL_DEPS= $(addsuffix .d, $(basename $(KERNEL_SRCS)))
OVMF=ovmf/bios64.bin
QEMU=qemu-system-x86_64

CLANG_WARNINGS = \
	-Wall -Wpedantic -Wextra -Wconditional-uninitialized -Wshorten-64-to-32 \
	-Werror \
	-Wno-keyword-macro \
	-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function \
    -Wno-gnu-binary-literal
C_STD = -std=c11
CXX_STD = -std=c++17

LIBCXX_FOR_KERNEL_INC_PATH=../third_party_root/include
LIBCXX_FOR_KERNEL_LIB_PATH=../third_party_root/lib

CFLAGS_KERNEL= \
	-target x86_64-unknown-none-elf \
	-fno-stack-protector -fno-exceptions -fshort-wchar \
	-mno-red-zone -mcmodel=large \
	-nostdlib \
	-D__ELF__ -D_LDBL_EQ_DBL \
	-I$(LIBCXX_FOR_KERNEL_INC_PATH) \
	$(CLANG_WARNINGS) $(C_STD)

LDFLAGS_KERNEL= \
				-L$(LIBCXX_FOR_KERNEL_LIB_PATH) -lm \
				-L../third_party_root/lib -lc \
				-static # --verbose

CXXFLAGS_FOR_TEST= \
				   $(CLANG_WARNINGS) $(CXX_STD) -g -stdlib=libc++ \
				   -DLIUMOS_TEST

FONTMAKER=../tools/fontmaker

$(FONTMAKER) :
	make -C $(dir $(FONTMAKER)) fontmaker

font.gen.cc : font.txt $(FONTMAKER)
	$(FONTMAKER) font.txt > font.gen.cc

# Test rules
test_% : %_test.cc Makefile
	$(CXX) $(CXXFLAGS_FOR_TEST) -o $*_test.bin $*_test.cc
	@./$*_test.bin

test_sheet : sheet_test.cc sheet.cc Makefile
	$(CXX) $(CXXFLAGS_FOR_TEST) -o sheet_test.bin sheet_test.cc sheet.cc asm.S
	@./sheet_test.bin

# Kernel rules

%.o : %.c Makefile
	$(LLVM_CC) $(CFLAGS_KERNEL) -g -c -o $*.o $*.c
%.d : %.c Makefile
	@$(LLVM_CC) $(CFLAGS_KERNEL) -MD -MF $@ -c -o $*.o $*.c >/dev/null 2>&1
	@echo 'DEP $@'

%.o : %.S Makefile
	$(LLVM_CC) $(CFLAGS_KERNEL) -c -o $*.o $*.S
%.d : %.S Makefile
	@touch $@ # generate dummy
	@echo 'DEP $@'

LIUMOS.ELF : $(KERNEL_OBJS) kernel.ld Makefile
	$(LLVM_LD_LLD) $(LDFLAGS_KERNEL) \
		-o LIUMOS.ELF -e kernel_entry -T kernel.ld $(KERNEL_OBJS)

newlib:
	CC=$(LLVM_CC) AR=$(LLVM_AR) RANLIB=$(LLVM_RANLIB) ./build_newlib.sh

libcxxabi:
	CC=$(LLVM_CC) \
	   CXX=$(LLVM_CXX) \
	   LD_LLD=$(LLVM_LD_LLD) \
	   AR=$(LLVM_AR) \
	   RANLIB=$(LLVM_RANLIB) \
	   ./build_libcxxabi.sh

libcxx:
	CC=$(LLVM_CC) \
	   CXX=$(LLVM_CXX) \
	   LD_LLD=$(LLVM_LD_LLD) \
	   AR=$(LLVM_AR) \
	   RANLIB=$(LLVM_RANLIB) \
	   ./build_libcxx.sh

run:
	make -C .. run

unittest:
	make test_ring_buffer
	make test_paging
	make test_xhci_trbring
	make test_sheet

clean :
	-rm -rf *.EFI
	-rm -rf *.lib
	-rm -rf *.o
	-rm -rf *.d
	-rm -rf *.bin

clean_third_party :
	-rm -rf third_party
	-rm -rf third_party_build
	-rm -rf third_party_root


format :
	clang-format -i *.cc *.h

commit :
	make -C .. commit

install :
	make -C .. install

sym : LIUMOS.ELF
	/usr/local/opt/binutils/bin/gobjdump -t LIUMOS.ELF | more

print_cc :
	@echo "$(LLVM_CC)"

dump : $(addsuffix .dump.txt, $(OBJS))

kerneldump :
	/usr/local/opt/llvm/bin/llvm-objdump --x86-asm-syntax=intel -d LIUMOS.ELF | more

FORCE : 

githash.cc: FORCE
	./update_githash_cc.sh

-include $(KERNEL_DEPS)
