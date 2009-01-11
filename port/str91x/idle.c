#include <gyros/private/port.h>

void
gyros__idle(void)
{
    __asm__ __volatile__(
        "mcr     15, 0, r0, cr7, c0, 4"
        ::: "memory"); 
}
