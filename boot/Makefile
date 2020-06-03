default: BOOTX64.EFI

include ../common.mk

CLANG_WARNINGS = \
	-Wall -Wpedantic -Wextra -Wconditional-uninitialized -Wshorten-64-to-32 -Wno-unused-parameter -Wno-unused-variable \
	-Werror \
	-Wno-keyword-macro \
	-Wno-incompatible-library-redeclaration

CFLAGS= \
	-target x86_64-pc-win32-coff \
	-fno-stack-protector -fno-exceptions -fshort-wchar \
	-mno-red-zone \
	-nostdlib \
	-nostdlibinc \
	-std=c11 \
	$(CLANG_WARNINGS) \
	-I. \
	-I./include \
	-I../vendor/newlib/include

LDFLAGS= \
    -nodefaultlib \
	-subsystem:efi_application \
	-entry:efi_main 

BOOTX64.EFI : *.c *.h *.S include/*.h include/efi/*.h Makefile
	$(LLVM_CC) $(CFLAGS) -O0 -c *.c *.S
	$(LLVM_LLD_LINK) $(LDFLAGS) -out:BOOTX64.EFI *.o

clean:
	-rm -rf *.o BOOTX64.EFI

