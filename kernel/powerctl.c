#include powerctl.h
void reboot() {
  outw(0xB004, 0x0006); // ACPI
  // Keyboard Reset Method
  while (inb(0x64) & 0x02);
  outb(0x64, 0xFE);
}
void shutdown(uint16_t pm1a_cnt_blk, uint16_t slp_typa) {
  outw(pm1a_cnt_blk, slp_typa | (1 << 13));
  outw(0x604, 0x2000);
}
extern hardoffasm
void hardoff() {
  hardoffasm();
}
