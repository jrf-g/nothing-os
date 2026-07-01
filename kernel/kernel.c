#include <stdint.h>
#include "kprint.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "autopit.h"
#include "multiboot.h"
#include "mm.h"
#include "nothingfs.h"
#define AUTOCLKEN 1

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    kclear();
    kprint("Kernel start.\n");

    if (magic != MULTIBOOT_MAGIC) {
        kprint("Bad multiboot magic.\n");
        for (;;) {}
    }

    gdt_init();
    pic_remap();
    idt_init();
    if (AUTOCLKEN) {
        autoclk();
    } else {
        pit_init(FREQ);
    }
    __asm__ volatile("sti");
    nfs_init();
    nfs_mkdir("/default");
    mm_init(mb_info_addr);

    kprint("Testing kmalloc...\n");
    void* a = kmalloc(128);
    void* b = kmalloc(256);
    kfree(a);
    void* c = kmalloc(64);

    kprint("Memory system online.\n");
    keyboard_init();
    shell();
    
    for (;;) {
        // idle
    }
}
