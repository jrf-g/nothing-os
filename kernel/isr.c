#include "isr.h"
#include "kprint.h"
#include "powerctl.h"
#include "panicchime.h"
extern void softoffisr(void);
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Intel Reserved Interrupt",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Critical Module Tampered With", "Bad Magic", "Out of Memory"
};
void isr_handler_c(struct isr_regs* r) {
    change_color(0x1E);
    kclear();
    kprint("[ NOTHINGOS PANIC ]\n");
    kprint("[ ***Exception: ");
    kprint(exception_messages[r->int_no]);
    kprint("*** ]\n\nSystem halted.\nPress key to reboot.\n");
    panic_chime();
    softoffisr();
    reboot();
    
}
