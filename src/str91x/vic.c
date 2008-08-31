#include <gyros/str91x/vic.h>

#include "str91x.h"

void
vic_set_isr(int irq, int priority, void (*isr)(void))
{
    struct VIC_regs *vic;
        
    if (irq < 16)
        vic = VIC0;
    else
        vic = VIC1;

    vic->INTECR |= 1 << (irq & 15); /* Disable the interrupt */

    vic->INTSR &= ~(1 << (irq & 15));
    vic->VAiR[priority] = (unsigned)isr;
    if (isr)
    {
        vic->VCiR[priority] = (1U << 5) | (irq & 0x0f);
        vic->INTER |= 1 << (irq & 15); /* Enable the interrupt */
    }
    else
        vic->VCiR[priority] = 0;
}
