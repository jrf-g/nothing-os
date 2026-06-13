CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -std=gnu99
LDFLAGS = -nostdlib -T linker.ld

OBJS = boot/multiboot_header.o kernel/kernel.o kernel/kprint.o

all: myos.bin

boot/multiboot_header.o: boot/multiboot_header.s
    $(AS) -o $@ $<

kernel/%.o: kernel/%.c
    $(CC) $(CFLAGS) -c -o $@ $<

myos.bin: $(OBJS)
    $(LD) $(LDFLAGS) -o $@ $(OBJS)

clean:
    rm -f $(OBJS) myos.bin
