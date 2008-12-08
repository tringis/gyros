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
#include <gyros/str91x/interrupt.h>

#include "str91x.h"
#include "../private.h"

#define PCLK            48000000

static gyros_abstime_t s_time_hi;
static uint16_t s_last_time_lo;

static void
tim3_isr(void)
{
    do
    {
        long dt = gyros__wake_sleeping_tasks();

        if (dt > 0x3fff)
            dt = 0x4000;

        TIM(3)->OC1R += dt;
        TIM(3)->SR &= ~(1U << 14);
    }
    while ((int16_t)(TIM(3)->OC1R - TIM(3)->CNTR) <= 0);
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    uint16_t time_lo = TIM(3)->CNTR, time_hi;

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

    TIM(3)->CR2 = PCLK / 1000000; /* 1 us */
    TIM(3)->CR2 |= 0x4000;        /* enable OC1 interrupt */
    TIM(3)->CNTR = 0;         /* reset count (exact value ignored) */
    TIM(3)->OC1R = 0x4000;    /* set period */
    TIM(3)->CR1 |= 0x8000;            /* start timer */
}
