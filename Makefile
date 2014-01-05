CC = gcc
CFLAGS = -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -m32 -c -g
DEPS = include/system.h
LDFLAGS := -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000

all: kernel.bin

kernel.bin: link.ld start.o main.o scrn.o gdt.o idt.o isrs.o irq.o timer.o kb.o mem.o math.o lba.o ext2.o fs.o paging.o str.o
	ld -melf_i386 -T link.ld -Map kernel.map -o kernel.bin start.o main.o scrn.o gdt.o idt.o isrs.o irq.o timer.o kb.o mem.o math.o lba.o ext2.o fs.o paging.o str.o

update: all
	./update_image.sh

run: update
	qemu-system-i386 -hda disk.img -monitor stdio

start.o: start.asm
	nasm -f elf -ostart.o start.asm

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.bin *.o
