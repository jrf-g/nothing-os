void delay(unsigned int time)
{
    unsigned int target = pit_ticks + time;
    while (pit_ticks < target) {}
}
