echo "Compiling...";
gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -m32 -c -o main.o main.c
gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -m32 -c -o scrn.o scrn.c
echo "Assembling...";
nasm -f aout -ostart.o start.asm
echo "Linking...";
ld -melf_i386 -T link.ld -o kernel.bin start.o main.o scrn.o
