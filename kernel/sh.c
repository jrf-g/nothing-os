#include <stdint.h>
#include "kprint.h"
#include "keyboard.h"
#include "pit.h"
#include "ramfs.h"
#include "powerctl.h"   // SKL power control module
// SKL is static kernel library
static int current_dir = 0; // root

// ------------------------------------------------------------
// Tokenizer
// ------------------------------------------------------------
static int tokenize(char* line, char* argv[], int max_args) {
    int argc = 0;

    while (*line && argc < max_args) {
        while (*line == ' ') line++;
        if (*line == 0) break;

        argv[argc++] = line;

        while (*line && *line != ' ') line++;

        if (*line == 0) break;

        *line++ = 0;
    }

    return argc;
}

// ------------------------------------------------------------
// Line reader
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// String compare
// ------------------------------------------------------------
static int streq(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

// ------------------------------------------------------------
// Built-in commands
// ------------------------------------------------------------
static void cmd_help(void) {
    kprint("Commands:\n");
    kprint("  help     - show this help\n");
    kprint("  clear    - clear the screen\n");
    kprint("  ticks    - show PIT tick count\n");
    kprint("  ls       - list directory\n");
    kprint("  pwd      - print working directory\n");
    kprint("  cd       - change directory\n");
    kprint("  mkdir    - create directory\n");
    kprint("  write    - write file\n");
    kprint("  cat      - read file\n");
    kprint("  rmv      - remove file or directory\n");
    kprint("  reboot   - reboot system\n");
    kprint("  shutdown - power off system\n");
}

static void cmd_clear(void) {
    kclear();
}

static void cmd_ticks(void) {
    kprint("Ticks: ");
    kprintf("%d\n", (int)pit_ticks());
}

static void cmd_reboot(void) {
    reboot();
}

static void cmd_shutdown(void) {
    shutdown();
}

// ------------------------------------------------------------
// Shell main loop
// ------------------------------------------------------------
void shell(void) {
    char line[256];
    char* argv[16];

    kprint("NothingOS Shell Ready.\n");

    while (1) {
        kprint("> ");
        read_line(line, sizeof(line));

        int argc = tokenize(line, argv, 16);
        if (argc == 0) continue;

        char* cmd = argv[0];

        if (strcmp(cmd, "ls") == 0) {
            nfs_list(argc > 1 ? argv[1] : "/");
        }
        else if (strcmp(cmd, "pwd") == 0) {
            cmd_pwd();
        }
        else if (strcmp(cmd, "cd") == 0) {
            if (argc < 2) {
                kprint("usage: cd <path>\n");
            } else {
                cmd_cd(argv[1]);
            }
        }
        else if (strcmp(cmd, "reboot") == 0) {
            cmd_reboot();
        }
        else if (strcmp(cmd, "shutdown") == 0) {
            cmd_shutdown();
        }
        else if (strcmp(cmd, "mkdir") == 0) {
            if (argc < 2) {
                kprint("usage: mkdir <path>\n");
                continue;
            }
            nfs_mkdir(argv[1]);
        }
        else if (strcmp(cmd, "write") == 0) {
            if (argc < 3) {
                kprint("usage: write <path> <text>\n");
                continue;
            }

            char buf[1024];
            buf[0] = 0;

            for (int i = 2; i < argc; i++) {
                strcat(buf, argv[i]);
                if (i + 1 < argc) strcat(buf, " ");
            }

            nfs_create(argv[1]);
            nfs_write(argv[1], buf, strlen(buf));
        }
        else if (strcmp(cmd, "cat") == 0) {
            if (argc < 2) {
                kprint("usage: cat <path>\n");
                continue;
            }

            char buf[2048];
            int n = nfs_read(argv[1], buf, sizeof(buf) - 1);
            if (n < 0) {
                kprint("read error\n");
                continue;
            }

            buf[n] = 0;
            kprint(buf);
            kputc('\n');
        }
        else if (strcmp(cmd, "rmv") == 0) {
            if (argc < 2) {
                kprint("usage: rmv <path>\n");
            } else {
                cmd_rmv(argv[1]);
            }
        }
        else if (strcmp(cmd, "help") == 0) {
            cmd_help();
        }
        else if (strcmp(cmd, "clear") == 0) {
            cmd_clear();
        }
        else if (strcmp(cmd, "ticks") == 0) {
            cmd_ticks();
        }
        else {
            kprint("Unknown command.\n");
        }
    }
}

// ------------------------------------------------------------
// Filesystem helpers
// ------------------------------------------------------------
static void cmd_ls(void) {
    ramfs_list();
}

static void cmd_write(const char* name, const char* text) {
    if (ramfs_create(name) != 0) {
        // ignore if exists
    }
    ramfs_write(name, text, (uint32_t)strlen(text));
}

static void cmd_cat(const char* name) {
    char buf[4096];
    int n = ramfs_read(name, buf, sizeof(buf) - 1);
    if (n < 0) {
        kprint("No such file.\n");
        return;
    }
    buf[n] = 0;
    kprint(buf);
    kputc('\n');
}

static void cmd_cd(const char* path) {
    if (!path) {
        current_dir = 0;
        return;
    }

    if (path[0] == '/') {
        int idx = nfs_lookup(path);
        if (idx < 0) { kprint("cd: no such directory\n"); return; }
        if (nfs_entries[idx].type != NF_DIR) { kprint("cd: not a directory\n"); return; }
        current_dir = idx;
        return;
    }

    char full[256];
    if (current_dir == 0)
        snprintf(full, sizeof(full), "/%s", path);
    else
        snprintf(full, sizeof(full), "%s/%s", pwd_buffer, path);

    int idx = nfs_lookup(full);
    if (idx < 0) { kprint("cd: no such directory\n"); return; }
    if (nfs_entries[idx].type != NF_DIR) { kprint("cd: not a directory\n"); return; }

    current_dir = idx;
}

static void build_path(int idx, char* out) {
    if (idx == 0) {
        strcpy(out, "/");
        return;
    }

    char temp[256];
    temp[0] = 0;

    while (idx != (uint32_t)-1) {
        char part[64];
        snprintf(part, sizeof(part), "/%s", nfs_entries[idx].name);
        strcat(part, temp);
        strcpy(temp, part);
        idx = nfs_entries[idx].parent;
    }

    strcpy(out, temp);
}

static void cmd_pwd(void) {
    char buf[256];
    build_path(current_dir, buf);
    kprint(buf);
    kputc('\n');
}

static void cmd_rmv(const char* path) {
    int r = nfs_remove(path);
    if (r == -1) kprint("rmv: no such file or directory\n");
    else if (r == -2) kprint("rmv: cannot remove root\n");
    else if (r == -3) kprint("rmv: directory not empty\n");
}
