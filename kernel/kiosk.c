#include "kiosk.h"
#include "busyclock.h"
#include "kprint.h"
void runkiosk() {
  kprint("Kiosk enter");
  wait(1000);
  kprint("Kiosk exit");
}
