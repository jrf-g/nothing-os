#include "pit.h"
#include <stdint.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_INPUT_HZ 1193182

static volatile uint64_t g_ticks = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

void pit_init(uint32_t frequency) {
    uint16_t divisor = (uint16_t)(PIT_INPUT_HZ / frequency);

    // Command: channel 0, access lobyte/hibyte, mode 3 (square wave), binary
    outb(PIT_COMMAND, 0x36);

    // Send divisor low byte, then high byte
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

    g_ticks = 0;
}

void pit_tick(void) {
    g_ticks++;
}

uint64_t pit_ticks(void) {
    return g_ticks;
}
