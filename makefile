all: kernel.bin

kernel.bin: boot.o kernel.o
	ld -m elf_i386 -T linker.ld -o kernel.bin boot.o kernel.o

boot.o: boot.S
	gcc -c -m32 boot.S -o boot.o

kernel.o: kernel.c
	gcc -c kernel.c -o kernel.o -ffreestanding -m32

clean:
	rm -f *.o kernel.bin