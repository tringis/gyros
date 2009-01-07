/**************************************************************************
 * Copyright (c) 2002-2008, Tobias Ringström <tobias@ringis.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/
#include <gyros/interrupt.h>
#include <gyros/private/port.h>
#include <gyros/target/config.h>
#include <gyros/time.h>

#include "str91x.h"

#define MAX_PERIOD      0x8000

#if !GYROS_CONFIG_DYNTICK
#error str91x target needs GYROS_CONFIG_DYNTICK
#endif

static gyros_abstime_t s_time_hi;
static unsigned short s_last_time_lo;

static void
tim3_isr(void)
{
    /* Disable the OC2 interrupt in cast it was enabled. */
    TIM(3)->CR2 &= ~TIM_CR2_OC2IE;

    /* Note that it's important to call gyros_time() here, because we
     * need to call it before 0xffff ticks have passed not to miss
     * time. */
    gyros__wake_timedout_tasks(gyros_time());
}

void
gyros__suspend_tick(void)
{
    /* We can't suspend the tick, because it would make gyros_time()
     * fail, so we set it to the maximum period. */
    TIM(3)->OC1R = s_last_time_lo + MAX_PERIOD;
    TIM(3)->SR &= ~TIM_SR_OCF1;
}

void
gyros__update_tick(gyros_abstime_t next_timeout)
{
    gyros_reltime_t dt = next_timeout - gyros_time();

    if (dt >= MAX_PERIOD)
    {
        TIM(3)->OC1R = s_last_time_lo + MAX_PERIOD;
        TIM(3)->SR &= ~TIM_SR_OCF1;
    }
    else if (dt <= 0)
    {
        /* Oops, we're late!  Make the interrupt happen by enabling
         * the OC2 interrupt (which is always on). */
        TIM(3)->CR2 |= TIM_CR2_OC2IE;
    }
    else
    {
        TIM(3)->OC1R = next_timeout;
        TIM(3)->SR &= ~TIM_SR_OCF1;

        if ((short)((unsigned short)next_timeout - (unsigned short)TIM(3)->CNTR) <= 0)
        {
            /* Oops, we missed the OC1 tick.  Make the interrupt happen by
             * enabling the OC2 interrupt (which is always on). */
            TIM(3)->CR2 |= TIM_CR2_OC2IE;
        }
    }
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    unsigned short time_lo = TIM(3)->CNTR, time_hi;

    if (time_lo < s_last_time_lo)
        ++s_time_hi;
    s_last_time_lo = time_lo;
    time_hi = s_time_hi;

    gyros_interrupt_restore(flags);

    return (time_hi << 16) | time_lo;
}

void
gyros__target_init(void)
{
    /* Enable the VIC */
    SCU->PCGR0 |= SCU_P0_VIC;
    SCU->PRR0 |= SCU_P0_VIC;

    /* Enable TIM2 and TIM3 */
    SCU->PCGR1 |= SCU_P1_TIM23;
    SCU->PRR1 |= SCU_P1_TIM23;

    TIM(3)->CR1 = 0;          /* disable timer */
    TIM(3)->CR2 = 0;
    TIM(3)->SR  = 0;          /* clear any interrupt events */

    gyros_target_set_isr(GYROS_IRQ_TIM3, tim3_isr);

    TIM(3)->CR2 = GYROS_CONFIG_STR91X_PCLK / GYROS_CONFIG_TIMER_RESOLUTION - 1;
    TIM(3)->CR2 |= 0x4000;        /* Enable OC1 interrupt */
    TIM(3)->OC1R = MAX_PERIOD;
    TIM(3)->OC2R = 1;             /* Make OC2 happen right away */
    TIM(3)->CNTR = 0;             /* Reset value (exact value ignored) */
    TIM(3)->CR1 |= 0x8000;        /* Start timer */
}
