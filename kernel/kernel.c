#include "gdt.h"
#include "kprint.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#def SPEED 100

void kernel_main(void) {
    gdt_init();
    kprint("GDT loaded.\n");

    kclear();
    kprint("Booting kernel...\n");

    pic_remap();
    idt_init();
    pit_init(SPEED);
    __asm__ volatile("sti");

    kprint("Interrupts loaded.\n");

    for (;;) {
    }
}
