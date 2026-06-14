#include <stdint.h>
#include "kprint.h"
#include "keyboard.h"
#include "pit.h"

static void read_line(char* buf, int max) {
    int i = 0;

    for (;;) {
        char c = kbd_getch();

        if (c == '\r' || c == '\n') {
            kputc('\n');
            buf[i] = 0;
            return;
        }

        if (c == '\b') {
            if (i > 0) {
                i--;
                kprint("\b \b");
            }
            continue;
        }

        if (i < max - 1) {
            buf[i++] = c;
            kputc(c);
        }
    }
}

static int streq(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

static void cmd_help(void) {
    kprint("Commands:\n");
    kprint("  help   - show this help\n");
    kprint("  clear  - clear the screen\n");
    kprint("  ticks  - show PIT tick count\n");
}

static void cmd_clear(void) {
    kclear();
}

static void cmd_ticks(void) {
    kprint("Ticks: ");
    kprintf("%d\n", (int)pit_ticks());
}

void shell(void) {
    char line[128];

    kprint("Shell ready.\n");

    for (;;) {
        kprint("> ");
        read_line(line, sizeof(line));

        if (line[0] == 0)
            continue;

        if (streq(line, "help")) {
            cmd_help();
        }
        else if (streq(line, "clear")) {
            cmd_clear();
        }
        else if (streq(line, "ticks")) {
            cmd_ticks();
        }
        else {
            kprint("Unknown command. Type 'help'.\n");
        }
    }
}
