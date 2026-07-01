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
    autoclk();
    keyboard_init();
    __asm__ volatile("sti");
    mm_init(mb_info_addr);
    nfs_init();
    nfs_mkdir("/default");

    kprint("Testing kmalloc...\n");
    void* a = kmalloc(128);
    kfree(a);
    void* b = kmalloc(128)
    kprint("Memory system online.\n");

    shell();
    
    for (;;) {}
}
