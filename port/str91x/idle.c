#include <gyros/private/port.h>

#ifdef __thumb__
#error This file must be compiled in ARM mode
#endif

void
gyros__idle(void)
{
    __asm__ __volatile__(
        "mcr     15, 0, r0, cr7, c0, 4"
        ::: "memory"); 
}
