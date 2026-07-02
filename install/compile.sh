#!/bin/bash
export CC=i686-elf-gcc
export LD=i686-elf-ld
export AS=nasm
nasm -f bin boot/boot.asm -o build/boot.bin
nasm -f elf32 boot/multiboot_header.s -o build/multiboot_header.o
nasm -f elf32 boot/stage2.asm -o build/stage2.o
i686-elf-gcc -ffreestanding -m32 -c kernel/*.c -I kernel -o build/
nasm -f elf32 kernel/*.s -o build/
i686-elf-ld -T linker.ld -o build/kernel.bin \
    build/*.o \
    build/multiboot_header.o \
    build/stage2.o
cat build/boot.bin build/kernel.bin > build/nothingos.img
