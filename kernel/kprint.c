#include "kprint.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM    ((uint16_t*)0xB8000)

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t text_color = 0x0F; // white on black

static void move_cursor(void) {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void change_color(uint8_t color) {
    text_color = color;
}

static void scroll(void) {
    if (cursor_y < VGA_HEIGHT)
        return;

    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_MEM[(y - 1) * VGA_WIDTH + x] =
                VGA_MEM[y * VGA_WIDTH + x];
        }
    }

    uint16_t blank = ((uint16_t)text_color << 8) | ' ';
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_MEM[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }

    cursor_y = VGA_HEIGHT - 1;
}

void kclear(void) {
    uint16_t blank = ((uint16_t)text_color << 8) | ' ';
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEM[i] = blank;
    cursor_x = cursor_y = 0;
    move_cursor();
}

void kputc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {
        VGA_MEM[cursor_y * VGA_WIDTH + cursor_x] =
            ((uint16_t)text_color << 8) | (uint8_t)c;
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    scroll();
    move_cursor();
}

void kprint(const char* s) {
    while (*s)
        kputc(*s++);
}

static void kprint_dec(uint32_t v) {
    char buf[11];
    int i = 0;
    if (v == 0) {
        kputc('0');
        return;
    }
    while (v > 0 && i < 10) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }
    while (i--)
        kputc(buf[i]);
}

static void kprint_hex(uint32_t v) {
    kprint("0x");
    for (int i = 7; i >= 0; i--) {
        uint8_t nib = (v >> (i * 4)) & 0xF;
        kputc(nib < 10 ? '0' + nib : 'A' + (nib - 10));
    }
}
/* minimal kprintf */
void kprintf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    while (*fmt) {
        if (*fmt != '%') {
            kputc(*fmt++);
            continue;
        }
        fmt++;
        switch (*fmt) {
        case 's': {
            const char* s = va_arg(ap, const char*);
            kprint(s ? s : "(null)");
        } break;
        case 'd': {
            int v = va_arg(ap, int);
            if (v < 0) {
                kputc('-');
                kprint_dec((uint32_t)(-v));
            } else {
                kprint_dec((uint32_t)v);
            }
        } break;
        case 'x': {
            uint32_t v = va_arg(ap, uint32_t);
            kprint_hex(v);
        } break;
        case 'c': {
            char c = (char)va_arg(ap, int);
            kputc(c);
        } break;
        case '%':
            kputc('%');
            break;
        default:
            kputc('%');
            kputc(*fmt);
            break;
        }
        fmt++;
    }

    va_end(ap);
}
