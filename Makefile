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
	-drive format=raw,file=fat:rw:mnt -net none \
	-serial tcp::1234,server,nowait \
	-serial stdio \
	-device qemu-xhci -device usb-mouse -device usb-kbd

src/LIUMOS.ELF : .FORCE
	make -C src LIUMOS.ELF

.FORCE :

files : src/LIUMOS.ELF .FORCE
	-rm -rf mnt
	mkdir -p mnt/
	cp -a dist/* mnt
	cp src/LIUMOS.ELF mnt/LIUMOS.ELF

run : files
	$(QEMU) $(QEMU_ARGS)

runc : files
	$(QEMU) $(QEMU_ARGS) -nographic

clean :
	rm -rf mnt
	make -C src clean

format :
	make -C src format
