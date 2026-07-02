#include "powerctl.h"
#include "kprint.c"
void badlib() {
  kprint("BAD MODULES HAVE BEEN LOADED; UNLOADING THEM NOW. THE SYSTEM WILL LOCK. PRESS ANY KET TO UNLOCK");
  __asm__ volatile("hlt");
  reboot();
}
