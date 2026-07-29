#include "kiosk.h"
#include "busyclock.h"
#include "kprint.h"
void runkiosk() {
  kprint("Kiosk enter");
  wait(1000, 1);
  kprint("Kiosk exit");
}
