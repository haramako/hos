default: src/LIUMOS.ELF

include common.mk

OVMF=ovmf/bios64.bin

ifeq ($(OS),Windows_NT)
QEMU="C:\Program Files\qemu\qemu-system-x86_64"
else
QEMU=qemu-system-x86_64
endif

QEMU_ARGS=\
	-bios $(OVMF) \
	-machine q35 -cpu qemu64 -smp 4 \
	-monitor telnet:127.0.0.1:1240,server,nowait \
	-m 2G \
	-net none \
	-drive format=raw,file=fat:rw:mnt \
	-serial tcp::1234,server,nowait \
	-serial stdio \
	-device qemu-xhci -device usb-mouse -device usb-kbd

BOOTFS = dist/BOOTFS.IMG

src/LIUMOS.ELF : .FORCE
	make -C src LIUMOS.ELF

.FORCE :

files : src/LIUMOS.ELF bootfs .FORCE
	-rm -rf mnt
	mkdir -p mnt/
	cp -a dist/* mnt
	cp src/LIUMOS.ELF mnt/LIUMOS.ELF

.PHONY: bootfs
bootfs:
	make -C app/hello
	mkdir -p fd
	cp app/hello/hello.elf fd
	rm -f $(BOOTFS)
	mformat -t 256 -h 1 -s 64 -C -i $(BOOTFS) ::
	mcopy -i $(BOOTFS) fd/* ::

run : # files
	$(QEMU) $(QEMU_ARGS)

runc : files
	$(QEMU) $(QEMU_ARGS) -gdb tcp::1192 -nographic

clean :
	rm -rf mnt
	make -C src clean

format :
	cd src && clang-format -i *.c *.h && gtags
	cd boot && clang-format -i *.c *.h include/*.h include/efi/*.h && gtags
