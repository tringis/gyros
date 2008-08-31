#include <gyros/str91x/vic.h>

#include "str91x.h"
#include "../private.h"

#define TIM_PERIOD 48000 /* PCLK / 1000 => 1 kHz */

unsigned long gyros__ticks;

static void
tick_isr(void)
{
    TIM(3)->OC1R += TIM_PERIOD;
    TIM(3)->SR &= ~(1U << 14);

    ++gyros__ticks;
    gyros__wake_sleeping_tasks();
}

void
gyros__tick_enable(void)
{
    SCU->PCGR1 |= SCU_P1_TIM23;
    SCU->PRR1 |= SCU_P1_TIM23;

    TIM(3)->CR1 = 0;          /* disable timer */
    TIM(3)->CR2 = 0;
    TIM(3)->SR  = 0;          /* clear any interrupt events */

    vic_set_isr(TIM_SRC_TIM3, 5, tick_isr);

    TIM(3)->CR2 = 0; /* PBLK */
    TIM(3)->CR2 |= 0x4000;        /* enable OC1 interrupt */
    TIM(3)->OC1R = TIM_PERIOD;    /* set period */
    TIM(3)->CNTR = 0;         /* reset count (exact value ignored) */
    TIM(3)->CR1 |= 0x8000;            /* start timer */
}
