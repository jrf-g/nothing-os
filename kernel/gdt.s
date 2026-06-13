.global gdt_flush

gdt_flush:
    movl 4(%esp), %eax   # load pointer to GDT descriptor
    lgdt (%eax)          # load GDT

    mov $0x10, %ax       # data segment selector (index 2)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    ljmp $0x08, $flush2  # far jump to reload CS
flush2:
    ret
