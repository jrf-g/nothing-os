#include "clk.h"
#include "pit.h"
#define FREQ 100
void autoclk {
  pit_init(FREQ);
}
