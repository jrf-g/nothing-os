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
#include "powerctl.h"
#include "usedkl.h"
void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    kclear();
    kprint("Kernel start.\n");

    if (magic != MULTIBOOT_MAGIC) {
        kprint("Bad multiboot magic. PRESS ANY KEY TO REBOOT\n");
        __asm__ volatile("hlt");
        reboot();
    }

    gdt_init();
    pic_remap();
    idt_init();
    autoclk();
    keyboard_init();
    __asm__ volatile("sti");
    initdkl();
    mm_init(mb_info_addr);
    nfs_init();
    nfs_mkdir("/default");

    kprint("Testing kmalloc...\n");
    void* a = kmalloc(128);
    kfree(a);
    a = NULL;
    void* b = safealloc(128);
    kprint("Memory system online.\n");

    shell();
    
    for (;;) {}
}
