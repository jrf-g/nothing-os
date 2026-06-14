#include <stdint.h>
#include "pic.h"
#include "pit.h"
#include "keyboard.h"
#include "kprint.h"

struct irq_regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

void irq_handler_c(struct irq_regs* r) {
    int irq = r->int_no - 32;

    switch (irq) {
        case 0:
            pit_tick();
            break;

        case 1:
            keyboard_irq_handler();
            // keyboard handler already sends EOI
            return;

        default:
            // Optional debug:
            // kprintf("Unhandled IRQ %d\n", irq);
            break;
    }

    pic_send_eoi(irq);
}
