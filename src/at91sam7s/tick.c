#include <gyros/at91sam7s/aic.h>

#include "../private.h"
#include "at91sam7s.h"

#define MCLK_FREQ    47923200

#define PIT_FREQ     (MCLK_FREQ / 16)
#define PIT_PERIOD   (PIT_FREQ / 1000)  /* cycles per 1 ms */

unsigned long gyros__ticks;

static void
pit_isr(void)
{
    if (AT91C_BASE_PITC->PITC_PISR & AT91C_SYSC_PITS)
    {
        uint32_t status = AT91C_BASE_PITC->PITC_PIVR;

        gyros__ticks += status >> 20;
        gyros__wake_sleeping_tasks();
    }
}

void
gyros__tick_enable(void)
{
    uint32_t dummy;

    aic_irq_add_sys_isr(pit_isr);

    dummy = AT91C_BASE_PITC->PITC_PIVR;
    AT91C_BASE_PITC->PITC_PIMR = AT91C_SYSC_PITEN | AT91C_SYSC_PITIEN |
        (PIT_PERIOD - 1);

    *AT91C_AIC_IECR = (1 << AT91C_ID_SYS);
}
