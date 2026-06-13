.global isr0
.global isr1
.global isr_common_stub

.extern isr_handler_c

.section .text

# Macro to define an ISR stub that pushes its vector number
.macro ISR_NOERR num
isr\num:
    pushl $0          # fake error code
    pushl $\num       # interrupt number
    jmp isr_common_stub
.endm

ISR_NOERR 0
ISR_NOERR 1
# ... repeat up to 31 as needed

isr_common_stub:
    pusha             # push general-purpose registers

    movl %ds, %ax
    pushl %eax

    mov $0x10, %ax    # kernel data segment
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    movl %esp, %eax   # pass pointer to stack frame
    pushl %eax
    call isr_handler_c
    add $4, %esp

    popl %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa

    add $8, %esp      # pop int number + error code
    iret
