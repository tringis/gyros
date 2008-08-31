#include <91x_scu.h>
#include <91x_vic.h>

#include "../private.h"

void
gyros__target_init(void)
{
    SCU->PCGRO |= __VIC;
    SCU->PRR0 |= __VIC;
}
