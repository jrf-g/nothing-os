#include <stdint.h>
#include "keyboard.h"
#include "pic.h"
#include "kprint.h"

#define KBD_DATA_PORT 0x60

static volatile char keybuf[128];
static volatile int head = 0, tail = 0;

static const char scancode_table[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0,   0,   0,   ' ',
    /* rest zero */
};

static inline uint8_t inb(uint16_t port) {
    uint8_t v;
    __asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

void keyboard_irq_handler(void) {
    uint8_t sc = inb(KBD_DATA_PORT);

    if (!(sc & 0x80)) { // key press
        char c = scancode_table[sc];
        if (c) {
            int next = (head + 1) & 127;
            if (next != tail) {
                keybuf[head] = c;
                head = next;
            }
        }
    }

    pic_send_eoi(1);
}

int kbd_getch(void) {
    for (;;) {
        if (head != tail) {
            char c = keybuf[tail];
            tail = (tail + 1) & 127;
            return c;
        }
    }
}

void keyboard_init(void) {
    kprint("Keyboard init.\n");
    // nothing else yet; IRQ1 already wired in IDT/IRQ code
}
