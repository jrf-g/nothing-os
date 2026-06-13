.global irq0
.global irq1
.global irq_common_stub
.extern irq_handler_c

.macro IRQ num
irq\num:
    pushl $0
    pushl $\num
    jmp irq_common_stub
.endm

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

irq_common_stub:
    pusha

    mov %ds, %ax
    pushl %eax

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov %esp, %eax
    pushl %eax
    call irq_handler_c
    add $4, %esp

    popl %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa
    add $8, %esp
    iret
