#include "idt.h"
#include "kprint.h"

#define IDT_ENTRIES 256

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr   idt_descriptor;

extern void isr0(void);
extern void isr1(void);
// ... up to however many you want (0–31 for exceptions)

static void idt_set_gate(int n, uint32_t handler) {
    idt[n].isr_low   = handler & 0xFFFF;
    idt[n].kernel_cs = 0x08;          // kernel code segment selector
    idt[n].reserved  = 0;
    idt[n].flags     = 0x8E;          // present, ring 0, 32-bit interrupt gate
    idt[n].isr_high  = (handler >> 16) & 0xFFFF;
}

static inline void lidt(struct idt_ptr* idtp) {
    __asm__ volatile ("lidt (%0)" :: "r"(idtp));
}

void idt_init(void) {
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base  = (uint32_t)&idt;

    // zero the table
    for (int i = 0; i < IDT_ENTRIES; ++i) {
        idt[i].isr_low = idt[i].kernel_cs = idt[i].reserved =
        idt[i].flags = idt[i].isr_high = 0;
    }

    // set exception handlers (0–31)
    idt_set_gate(0,  (uint32_t)isr0);
    idt_set_gate(1,  (uint32_t)isr1);
    // ... repeat for isr2..isr31 when you add them

    lidt(&idt_descriptor);
}
