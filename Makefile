.PHONY: run debug-run clean

run: clean xieu.iso
	qemu-system-x86_64 -cdrom xieu.iso

debug-run: clean xieu.iso
	qemu-system-x86_64 -cdrom xieu.iso \
		-M accel=tcg,smm=off -d int -no-shutdown -s -S

xieu.iso: build/kernel.bin
	mkdir -p iso-root/boot/grub
	cp build/kernel.bin iso-root/boot/kernel.bin
	cp grub.cfg iso-root/boot/grub/grub.cfg
	grub2-mkrescue -o $@ iso-root

build/kernel.bin: build/boot.o build/main.o
	gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

build/boot.o: src/boot.S
	@mkdir -p build
	as $< -o $@

build/main.o: src/main.c
	@mkdir -p build
	gcc -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	rm -rf *.o kernel.bin xieu.iso iso-root build
