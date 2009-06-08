/**************************************************************************
 * Copyright (c) 2002-2009, Tobias Ringström <tobias@ringis.se>
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

#define GTIM            TIM(GYROS_CONFIG_STR91X_TIMER)

#if GYROS_CONFIG_DYNTICK
#define MAX_PERIOD      0x8000
static gyros_abstime_t s_time_hi;
static unsigned short s_last_time_lo;
#else
static gyros_abstime_t s_time;
#endif

static void
tim_isr(void)
{
    /* Clear the OC1 interrupt. */
    GTIM->SR &= ~TIM_SR_OCF1;

#if GYROS_CONFIG_DYNTICK
    /* Disable the OC2 interrupt in case it was enabled. */
    GTIM->CR2 &= ~TIM_CR2_OC2IE;

    /* Note that it's important to call gyros_time() here when using
     * dynticks, because we need to call it before 0xffff ticks have
     * passed not to miss time. */
    gyros__wake_timedout_tasks(gyros_time());
#else
    /* Move OC1 forward one timer period. */
    GTIM->OC1R += GYROS_CONFIG_TIMER_PERIOD;
    gyros__wake_timedout_tasks(++s_time);
#endif
}

#if GYROS_CONFIG_DYNTICK
void
gyros__dyntick_suspend(void)
{
    /* We can't suspend the tick, because it would make gyros_time()
     * fail, so we set it to the maximum period. */
    GTIM->OC1R = s_last_time_lo + MAX_PERIOD;
}

void
gyros__dyntick_set(gyros_abstime_t now, gyros_abstime_t next_timeout)
{
    gyros_reltime_t dt = next_timeout - now;

    if (dt >= MAX_PERIOD)
        GTIM->OC1R = s_last_time_lo + MAX_PERIOD;
    else
    {
        GTIM->OC1R = next_timeout;

        /* If dt is small enough that CNTR may already be past the
         * value we just set OC1R to, we check if it did indeed miss
         * the value, and in that case force a timer interrupt using
         * OC2 (which is always on). */
        if (dt < MAX_PERIOD / 4 &&
            (short)((unsigned short)next_timeout - GTIM->CNTR) <= 0)
        {
            GTIM->CR2 |= TIM_CR2_OC2IE;
        }
    }
}
#endif

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
#if GYROS_CONFIG_DYNTICK
    unsigned short time_lo = GTIM->CNTR;
    gyros_abstime_t time_hi;

    if (time_lo < s_last_time_lo)
        ++s_time_hi;
    s_last_time_lo = time_lo;
    time_hi = s_time_hi;

    gyros_interrupt_restore(flags);

    return (time_hi << 16) | time_lo;
#else
    gyros_abstime_t time = s_time;

    gyros_interrupt_restore(flags);

    return time;
#endif
}

void
gyros__target_init(void)
{
    /* Enable the VIC */
    SCU->PCGR0 |= SCU_P0_VIC;
    SCU->PRR0 |= SCU_P0_VIC;

#if GYROS_CONFIG_STR91X_TIMER < 2
    /* Enable TIM0 and TIM1 */
    SCU->PCGR1 |= SCU_P1_TIM01;
    SCU->PRR1 |= SCU_P1_TIM01;
#else
    /* Enable TIM2 and TIM3 */
    SCU->PCGR1 |= SCU_P1_TIM23;
    SCU->PRR1 |= SCU_P1_TIM23;
#endif

    GTIM->CR1 = 0;          /* disable timer */
    GTIM->CR2 = 0;
    GTIM->SR  = 0;          /* clear any interrupt events */

    gyros_target_set_isr(GYROS_IRQ_TIM0 + GYROS_CONFIG_STR91X_TIMER, tim_isr);

    GTIM->CR2 = GYROS_CONFIG_STR91X_PCLK / GYROS_CONFIG_TIMER_RESOLUTION - 1;
    GTIM->CR2 |= 0x4000;        /* Enable OC1 interrupt */
#if GYROS_CONFIG_DYNTICK
    GTIM->OC1R = MAX_PERIOD;
    GTIM->OC2R = 1;             /* Make OC2 happen right away */
#else
    GTIM->OC1R = GYROS_CONFIG_TIMER_PERIOD;
#endif
    GTIM->CNTR = 0;             /* Reset value (exact value ignored) */
    GTIM->CR1 |= 0x8000;        /* Start timer */

    gyros__interrupt_enable();
}
