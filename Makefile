default: files

include common.mk

BOOTFS=mnt/BOOTFS.IMG
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
	mkdir -p mnt/EFI/BOOT
	cp boot/BOOTX64.EFI mnt/EFI/BOOT/BOOTX64.EFI
	cp src/KERNEL.ELF mnt/KERNEL.ELF

boot/BOOTX64.EFI: .FORCE
	$(MAKE) -C boot BOOTX64.EFI

$(BOOTFS): .FORCE
	mkdir -p mnt
	$(MAKE) -C app/hello
	$(MAKE) -C app/stdlib
	rm -f $(BOOTFS)
	mformat -t 256 -h 1 -s 64 -C -i $(BOOTFS) ::
	mcopy -i $(BOOTFS) app/hello/hello.elf app/stdlib/stdlib.elf bootfs_data/* ::

run : files
	$(QEMU) $(QEMU_ARGS)

runc : files
	$(QEMU) $(QEMU_ARGS) -nographic

clean :
	rm -rf mnt
	$(MAKE) -C src clean
	$(MAKE) -C boot clean

format :
	cd src && clang-format -i *.c *.h **/*.h **/*.c && gtags
	cd boot && clang-format -i *.c *.h **/*.h && gtags

gdb:
	gdb -x debug_kernel.gdb
