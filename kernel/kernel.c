#include <stdint.h>
#include "kprint.h"

void kernel_main(void) {
    kclear();
    kprint("Hello from the jrf-g kernel!\n");

    for (;;)
        ; // hang
}
