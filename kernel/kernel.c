#include "kprint.h"
#include "idt.h"

void kernel_main(void) {
    kclear();
    kprint("Setting up IDT...\n");
    idt_init();
    kprint("IDT ready.\n");

    for (;;)
        ;
}
