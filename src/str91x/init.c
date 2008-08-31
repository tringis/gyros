#include "str91x.h"
#include "../private.h"

void
gyros__target_init(void)
{
    SCU->PCGR0 |= SCU_P0_VIC;
    SCU->PRR0 |= SCU_P0_VIC;
}
