#include <gyros/str91x/vic.h>
#include <91x_scu.h>
#include <91x_tim.h>
#include <91x_vic.h>

#include "../private.h"

#define TIM_PERIOD 48000 /* PCLK / 1000 => 1 kHz */

unsigned long gyros__ticks;

static void
tick_isr(void)
{
    TIM3->OC1R += TIM_PERIOD;
    TIM3->SR &= ~TIM_FLAG_OC1;

    ++gyros__ticks;
    gyros__wake_sleeping_tasks();
}

void
gyros__tick_enable(void)
{
    SCU->PCGR1 |= __TIM23;
    SCU->PRR1 |= __TIM23;

    TIM3->CR1 = 0;          /* disable timer */
    TIM3->CR2 = 0;
    TIM3->SR  = 0;          /* clear any interrupt events */

    SCU->PCGRO |= __VIC;
    SCU->PRR0 |= __VIC;

    vic_set_isr(TIM3_ITLine, 5, tick_isr);

    TIM3->CR2 = 0; /* PBLK */
    TIM3->CR2 |= 0x4000;        /* enable OC1 interrupt */
    TIM3->OC1R = TIM_PERIOD;    /* set period */
    TIM3->CNTR = 0;         /* reset count (exact value ignored) */
    TIM3->CR1 |= 0x8000;            /* start timer */
}
