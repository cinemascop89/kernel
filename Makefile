CC = gcc
CFLAGS = -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -m32 -c -g
DEPS = include/system.h
LDFLAGS := -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000

all: kernel.bin

kernel.bin: link.ld start.o main.o scrn.o gdt.o idt.o isrs.o irq.o timer.o kb.o mem.o math.o lba.o ext2.o fs.o paging.o
	ld -melf_i386 -T link.ld -Map kernel.map -o kernel.bin start.o main.o scrn.o gdt.o idt.o isrs.o irq.o timer.o kb.o mem.o math.o lba.o ext2.o fs.o paging.o

update: all
	./update_image.sh

run: update
	qemu-system-i386 -hda disk.img -monitor stdio

start.o: start.asm
	nasm -f elf -ostart.o start.asm

main.o: main.c
	$(CC) $(CFLAGS) -o main.o main.c

scrn.o: scrn.c
	$(CC) $(CFLAGS) -o scrn.o scrn.c

gdt.o: gdt.c
	$(CC) $(CFLAGS) -o gdt.o gdt.c

idt.o: idt.c
	$(CC) $(CFLAGS) -o idt.o idt.c

isrs.o: isrs.c
	$(CC) $(CFLAGS) -o isrs.o isrs.c

irq.o: irq.c
	$(CC) $(CFLAGS) -o irq.o irq.c

timer.o: timer.c
	$(CC) $(CFLAGS) -o timer.o timer.c

mem.o: mem.c
	$(CC) $(CFLAGS) -o mem.o mem.c

kb.o: kb.c
	$(CC) $(CFLAGS) -o kb.o kb.c

math.o: math.c
	$(CC) $(CFLAGS) -o math.o math.c

lba.o: lba.c
	$(CC) $(CFLAGS) -o lba.o lba.c

ext2.o: ext2.c
	$(CC) $(CFLAGS) -o ext2.o ext2.c

fs.o: fs.c
	$(CC) $(CFLAGS) -o fs.o fs.c

paging.o: paging.c
	$(CC) $(CFLAGS) -o paging.o paging.c
clean:
	rm -f *.bin *.o
