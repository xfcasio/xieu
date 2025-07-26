CC = gcc
CFLAGS = -std=gnu11 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel -Wall -O2 -Ilimine -g
LDFLAGS = -nostdlib -static -T x86_64.lds

override CFLAGS += \
    
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: run xieu.iso clean

.PHONY: run
run: clean ovmf/ovmf-code-x86_64.fd xieu.iso
	qemu-system-x86_64 \
		-M accel=tcg,smm=off -d int\
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on \
		-cdrom xieu.iso

.PHONY: debug-run
debug-run: clean ovmf/ovmf-code-x86_64.fd xieu.iso
	qemu-system-x86_64 \
		-M accel=tcg,smm=off -d int -no-shutdown -s -S\
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on \
		-cdrom xieu.iso

ovmf/ovmf-code-x86_64.fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-x86_64.fd

boot/kernel.elf: $(OBJECTS)
	mkdir -p boot
	$(CC) -Ilimine $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

xieu.iso: boot/kernel.elf
	mkdir -p iso_root/boot
	cp boot/kernel.elf limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/
	xorriso \
		-as mkisofs \
		-b boot/limine-bios-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		--efi-boot boot/limine-uefi-cd.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label iso_root \
		-o xieu.iso
	./limine/limine bios-install xieu.iso

.PHONY: clean
clean:
	rm -f $(OBJECTS) boot/kernel.elf xieu.iso
	rm -rf iso_root
