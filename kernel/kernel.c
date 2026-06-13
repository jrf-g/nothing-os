#include "kprint.h"
#include "idt.h"
#include "pic.h"

void kernel_main(void) {
    kclear();
    kprint("Booting kernel...\n");
    // --- INTERRUPT SYSTEM SETUP ---
    pic_remap();                 // Move IRQs to 32–47
    idt_init();                  // Load IDT with ISRs + IRQs
    __asm__ volatile("sti");     // Enable interrupts globally
    kprint("Interrupts enabled.\n");
    for (;;) {
        // idle loop
    }
}
