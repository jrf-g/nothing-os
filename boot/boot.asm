; boot/boot.asm - 16-bit bootloader
; Assembled with: nasm -f bin boot/boot.asm -o boot.bin

[org 0x7C00]
[bits 16]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl      ; BIOS gives boot drive in DL

    ; --- Load kernel from disk ---
    ; Load N sectors starting at LBA 1 (CHS: head 0, track 0, sector 2)
    ; into physical address 0x00100000 (ES:BX = 0x1000:0x0000)

    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02              ; BIOS read sectors
    mov al, 32                ; number of sectors to read (adjust as needed)
    mov ch, 0                 ; cylinder 0
    mov cl, 2                 ; sector 2 (sector 1 is bootloader)
    mov dh, 0                 ; head 0
    mov dl, [boot_drive]      ; boot drive
    int 0x13
    jc disk_error

    ; --- Enable A20 (fast A20 via port 0x92) ---
    in   al, 0x92
    or   al, 00000010b
    out  0x92, al

    ; --- Set up a minimal GDT for protected mode ---
    lgdt [gdt_descriptor]

    ; --- Enter protected mode ---
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    jmp 0x08:protected_entry   ; far jump to flush CS

disk_error:
    ; Simple hang on disk error
    cli
    hlt
    jmp $

; ---------------- 32-bit code ----------------
[bits 32]

protected_entry:
    mov ax, 0x10               ; data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x00200000        ; temporary stack at 2 MiB

    ; Jump to kernel entry at 0x00100000
    jmp 0x00100000

; ---------------- GDT ----------------

gdt_start:
    ; Null descriptor
    dq 0x0000000000000000

    ; Code segment: base=0, limit=4GB, access=0x9A, gran=0xCF
    dq 0x00CF9A000000FFFF

    ; Data segment: base=0, limit=4GB, access=0x92, gran=0xCF
    dq 0x00CF92000000FFFF

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

boot_drive: db 0

times 510-($-$$) db 0
dw 0xAA55
