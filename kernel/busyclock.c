void wait(int time, int schematic) {
  for (i = 0; i < time; i++) {
    if (schematic != 0) {
      __asm__ volatile( "pause" ::: "memory" );
    } else {
      __asm__ volatile( "nop" ::: "memory" );
    }
  }
}
