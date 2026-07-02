#include "powerctl.h"
#include "kprint.c"
void badlib() {
  kprint("BAD MODULES HAVE BEEN LOADED; UNLOADING THEM NOW. THE SYSTEM WILL LOCK.");
  __asm__ volatile("int $32");
}
