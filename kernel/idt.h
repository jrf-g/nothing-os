#pragma once
#include <stdint.h>

struct idt_entry {
    uint16_t isr_low;   // lower 16 bits of handler address
    uint16_t kernel_cs; // code segment selector
    uint8_t  reserved;  // must be 0
    uint8_t  flags;     // type & attributes
    uint16_t isr_high;  // upper 16 bits of handler address
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_init(void);
