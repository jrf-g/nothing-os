#include <stdint.h>
#include "kprint.h"

static uint16_t* const VGA = (uint16_t*)0xB8000;
static uint8_t row = 0, col = 0;

static void putchar_at(char c, uint8_t r, uint8_t c2) {
    uint16_t idx = r * 80 + c2;
    VGA[idx] = (uint16_t)c | (0x07 << 8); // light grey on black
}

void kclear(void) {
    for (uint16_t i = 0; i < 80 * 25; ++i)
        VGA[i] = ' ' | (0x07 << 8);
    row = col = 0;
}

void kprint(const char* s) {
    while (*s) {
        if (*s == '\n') {
            row++;
            col = 0;
        } else {
            putchar_at(*s, row, col++);
            if (col >= 80) {
                col = 0;
                row++;
            }
        }
        s++;
    }
}
