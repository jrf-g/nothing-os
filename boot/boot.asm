; boot/boot.asm

[org 0x7C00]
[bits 16]

start:
    mov [boot_drive], dl

    mov ax, 0
    mov ds, ax
    mov es, ax

    ; Load Stage‑2 (10 sectors) to 0x0000:0x8000
    mov ax, 0
    mov es, ax
    mov bx, 0x8000

    mov ah, 0x02
    mov al, 10
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc $

    jmp 0x0000:0x8000

boot_drive db 0

times 510-($-$$) db 0
dw 0xAA55
