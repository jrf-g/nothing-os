#include powerctl.h
#define USE_PMXB
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
static inline void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}
void acpi_initialize() {
  outb(0xB2, 0xA0);
}
void reboot() {
  outw(0xB004, 0x0006); // reboot on 1990s-2000s PCs and QEMU
  outb(0xCF9, 0x06); // reboot on mid-era PCs using chipset hacks
  outb(0x64, 0xFE); // reboot everywhere fallback
}
void shutdown() {
    outw(0x0404, (0x7 << 10) | (1 << 13));
    #ifdef USE_PMXB
    outw(0x0604, (0x7 << 10) | (1 << 13));
    #endif
}
extern hardoffasm
void hardoff() {
  hardoffasm(); // invoke assembly shutdown routine
}
