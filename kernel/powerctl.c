#include powerctl.h
void reboot() {
  outw(0xB004, 0x0006); // reboot on 1990s-2000s PCs and QEMU
  outb(0xCF9, 0x06); // reboot on mid-era PCs using chipset hacks
  outb(0x64, 0xFE); // reboot everywhere fallback
}
void shutdown(uint16_t pm1a_cnt_blk, uint16_t slp_typa) {
  outw(0x604, 0x2000);
  outw(pm1a_cnt_blk, slp_typa | (1 << 13));
  hardoff(); // if we cant shut down, then halt
  reboot(); // if we cant halt, then reboot
}
extern hardoffasm
void hardoff() {
  hardoffasm(); // invoke assembly shutdown routine
}
