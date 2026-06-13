#include "irq.h"
#include "pic.h"
#include "kprint.h"
#include "pit.h"
#def TICK

void irq_handler_c(struct irq_regs* r) {
    int irq = r->int_no - 32;

    if (irq == 0) {
        pit_tick();
        #ifdef TICK
        if ((pit_ticks() % 100) == 0) kprint("tick\n");
        #endif
    }

    pic_send_eoi(irq);
}
