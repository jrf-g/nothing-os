    .section .multiboot
    .align 4
    .long 0x1BADB002          # magic
    .long 0x0                 # flags
    .long -(0x1BADB002)       # checksum

    .section .text
    .global _start
_start:
    call kernel_main
1:  jmp 1b
