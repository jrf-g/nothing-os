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
#define SAFEMODE = 0
uint32_t fancy = 0;
typedef void* MemAddr;
typedef unsigned char* BytesPointer;
void chime() {
    beep(440, 110)
}
void memtest(void* ptr, unsigned int size)
{
    /* Fill with ones (0xFF) */
    BytesPointer pc = (BytesPointer)ptr;
    for (unsigned int icxa = 0; icxa < size; icxa++) {
        pc[icxa] = 0xFF;
    }

    /* Zero out again */
    for (unsigned int icxb = 0; icxb < size; icxb++) {
        pc[icxb] = 0x00;
    }
}
void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    kclear();
    kprint("Kernel start.\n");
    fancy = !SAFEMODE
    if (!fancy) {
        kprint("SAFE MODE ENABLED.");
    }
    if (magic != MULTIBOOT_MAGIC) {
        kprint("Bad multiboot magic. PRESS ANY KEY TO REBOOT\n");
        __asm__ volatile("int 33");
        // reboot not needed. the ISR handles that
    }

    gdt_init();
    pic_remap();
    idt_init();
    if (fancy)  {
        autoclk();
    } else {
        pit_init(FREQ);
    }
    keyboard_init();
    __asm__ volatile("sti");
    if (fancy) {
        initdkl();
    }
    mm_init(mb_info_addr);
    nfs_init();
    nfs_mkdir("/default");

    kprint("Testing kmalloc...\n");
    MemAddr a = kmalloc(STARTMEMINT);
    kfree(a);
    a = NULL;
    MemAddr b = safealloc(STARTMEMINT);
    memtest(b, STARTMEMINT);
    kprint("Memory system online.\n" STARTMEMSTR "(" MEMUNIT "(" MEMPLURAL "))" "allocated\n");

    chime();
    shell();
    
    for (;;) {}
}
