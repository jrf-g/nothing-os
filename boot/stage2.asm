; boot/stage2.asm
; Loaded by Stage‑1 at 0x0000:0x8000

[org 0x8000]
[bits 16]

stage2_start:
    mov si, msg_stage2
    call print_string

    ; Load kernel (32 sectors) to 0x100000
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 32
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc load_fail

    ; Enable A20
    in al, 0x92
    or al, 00000010b
    out 0x92, al

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Enter protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:pm_entry

load_fail:
    mov si, msg_fail
    call print_string
    cli
    hlt

; ---------------- 32-bit mode ----------------
[bits 32]

pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x200000

    jmp 0x00100000

; ---------------- GDT ----------------

gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; ---------------- Helpers ----------------
[bits 16]

print_string:
    mov ah, 0x0E
.print_loop:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .print_loop
.done:
    ret

msg_stage2 db "Stage 2 loaded!", 0
msg_fail   db "Kernel load failed!", 0

boot_drive db 0
