#include "time.h"
void beep(unsigned int freq, unsigned int duration)
{
    unsigned int div = 1193180 / freq;

    /* Program PIT channel 2 */
    outb(0x43, 0xB6);
    outb(0x42, (unsigned char)(div & 0xFF));
    outb(0x42, (unsigned char)((div >> 8) & 0xFF));

    /* Enable speaker (bits 0 and 1) */
    unsigned char tmp = inb(0x61);
    outb(0x61, tmp | 0x03);

    /* Wait */
    delay(duration);

    /* Disable speaker (clear bit 1) */
    tmp = inb(0x61);
    outb(0x61, tmp & ~0x02);
}
