#include "irq.h"
#include "pic.h"
#include "kprint.h"

void irq_handler_c(struct irq_regs* r) {
    int irq = r->int_no - 32;

    // TEMP: print IRQ number
    kprint("IRQ ");
    char buf[4];
    buf[0] = '0' + irq;
    buf[1] = '\n';
    buf[2] = 0;
    kprint(buf);

    pic_send_eoi(irq);
}
