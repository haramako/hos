default: files

include common.mk

BOOTFS=dist/BOOTFS.IMG
OVMF=ovmf/bios64.bin
QEMU=qemu-system-x86_64

QEMU_ARGS=\
	-bios $(OVMF) \
	-machine q35 -cpu qemu64 -smp 4 \
	-monitor telnet:127.0.0.1:1240,server,nowait \
	-m 2G \
	-net none \
	-drive format=raw,file=fat:rw:mnt \
	-serial tcp::1234,server,nowait \
	-serial stdio \
	-gdb tcp::1192 \
	-device qemu-xhci -device usb-mouse -device usb-kbd


src/KERNEL.ELF : .FORCE
	$(MAKE) -C src KERNEL.ELF

.FORCE :

files : src/KERNEL.ELF boot/BOOTX64.EFI $(BOOTFS) .FORCE
	-rm -rf mnt
	mkdir -p mnt/
	cp -a dist/* mnt
	cp src/KERNEL.ELF mnt/KERNEL.ELF

boot/BOOTX64.EFI: .FORCE
	$(MAKE) -C boot BOOTX64.EFI

$(BOOTFS): .FORCE
	$(MAKE) -C app/hello
	mkdir -p fd
	cp app/hello/hello.elf fd
	rm -f $(BOOTFS)
	mformat -t 256 -h 1 -s 64 -C -i $(BOOTFS) ::
	mcopy -i $(BOOTFS) fd/* ::

run : files
	$(QEMU) $(QEMU_ARGS)

runc : files
	$(QEMU) $(QEMU_ARGS) -nographic

clean :
	rm -rf mnt
	$(MAKE) -C src clean
	$(MAKE) -C boot clean

format :
	cd src && clang-format -i *.c *.h **/*.h && gtags
	cd boot && clang-format -i *.c *.h **/*.h && gtags
