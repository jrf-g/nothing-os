#include "pic.h"
#include <stdint.h>

#define PIC1        0x20
#define PIC2        0xA0
#define PIC1_CMD    PIC1
#define PIC1_DATA   (PIC1+1)
#define PIC2_CMD    PIC2
#define PIC2_DATA   (PIC2+1)

#define ICW1_INIT   0x10
#define ICW1_ICW4   0x01
#define ICW4_8086   0x01

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

void pic_remap(void) {
    uint8_t a1, a2;

    __asm__ volatile ("inb %1, %0" : "=a"(a1) : "Nd"(PIC1_DATA));
    __asm__ volatile ("inb %1, %0" : "=a"(a2) : "Nd"(PIC2_DATA));

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

    outb(PIC1_DATA, 0x20); // Master PIC vector offset
    outb(PIC2_DATA, 0x28); // Slave PIC vector offset

    outb(PIC1_DATA, 4);    // Tell Master PIC about Slave at IRQ2
    outb(PIC2_DATA, 2);    // Tell Slave PIC its cascade identity

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void pic_send_eoi(int irq) {
    if (irq >= 8)
        outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}
