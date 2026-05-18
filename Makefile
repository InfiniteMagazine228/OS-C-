# Trình biên dịch giả lập môi trường x86 (32-bit) tương thích với GitHub Actions
CC = i686-linux-gnu-g++
AS = i686-linux-gnu-as
LD = i686-linux-gnu-ld

CFLAGS = -m32 -nostdlib -fno-builtin -fno-exceptions -fno-rtti
LDFLAGS = -melf_i386 -T linker.ld

all: myos.iso

boot.o: boot.s
	$(AS) --32 boot.s -o boot.o

kernel.o: kernel.cpp
	$(CC) $(CFLAGS) -c kernel.cpp -o kernel.o

myos.bin: boot.o kernel.o
	$(LD) $(LDFLAGS) boot.o kernel.o -o myos.bin

myos.iso: myos.bin
	mkdir -p iso/boot/grub
	cp grub.cfg iso/boot/grub/grub.cfg
	cp myos.bin iso/boot/myos.bin
	grub-mkrescue -o myos.iso iso

clean:
	rm -f *.o myos.bin myos.iso
	rm -rf iso
