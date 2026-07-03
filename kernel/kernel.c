#include <stdint.h>
#include "kprint.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "autopit.h"
#include "multiboot.h"
#include "mm.h"
#include "nothingfs.h"
#include "powerctl.h"
#include "usedkl.h"
#include "startmem.h"
#include "audio.h"
typedef void* MemAddr;
void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    kclear();
    kprint("Kernel start.\n");

    if (magic != MULTIBOOT_MAGIC) {
        kprint("Bad multiboot magic. PRESS ANY KEY TO REBOOT\n");
        __asm__ volatile("int $33");
        // reboot not needed. the ISR handles that
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
    MemAddr a = kmalloc(STARTMEMINT);
    kfree(a);
    a = NULL;
    MemAddr b = safealloc(STARTMEMINT);
    kprint("Memory system online.\n" STARTMEMSTR MEMUNIT MEMPLURAL "allocated\n");
    beep();
    shell();
    
    for (;;) {}
}
