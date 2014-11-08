CROSS_PATH := ~/opt/cross/bin
CC = $(CROSS_PATH)/i586-elf-gcc
ASM = $(CROSS_PATH)/i586-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I./include -c -g

all: kernel.bin

kernel.bin: linker.ld boot.o kernel.o scrn.o str.o math.o io.o descriptor_tables.o isr.o gdt.o interrupt.o isr.o irq.o timer.o kb.o
	$(CC) -T linker.ld -o kernel.bin -ffreestanding -O2 -nostdlib boot.o kernel.o scrn.o str.o descriptor_tables.o gdt.o interrupt.o isr.o io.o irq.o timer.o kb.o -lgcc

run: all
	#./update_image.sh
	#qemu-system-i386 -hda disk.img -monitor stdio -no-reboot
	qemu-system-i386 -kernel kernel.bin -monitor stdio -no-reboot


boot.o: boot.s
	$(ASM) boot.s -g -o boot.o

gdt.o: gdt.s
	$(ASM) gdt.s -g -o gdt.o

interrupt.o: interrupt.s
	$(ASM) interrupt.s -g -o interrupt.o

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.bin *.o
