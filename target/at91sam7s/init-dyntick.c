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
#include <gyros/private/target.h>

#include "at91sam7s.h"

#define MAX_PERIOD      0x8000

#if GYROS_CONFIG_TIMER_NUM == 0
#  define TC             AT91C_BASE_TC0
#elif GYROS_CONFIG_TIMER_NUM == 1
#  define TC             AT91C_BASE_TC1
#elif GYROS_CONFIG_TIMER_NUM == 2
#  define TC             AT91C_BASE_TC2
#else
#  error Unsupported value for GYROS_CONFIG_TIMER_NUM
#endif

#if GYROS_CONFIG_TIMER_DIV == 2
#define TIMER_CLOCK      AT91C_TC_CLKS_TIMER_DIV1_CLOCK
#elif GYROS_CONFIG_TIMER_DIV == 8
#define TIMER_CLOCK      AT91C_TC_CLKS_TIMER_DIV2_CLOCK
#elif GYROS_CONFIG_TIMER_DIV == 32
#define TIMER_CLOCK      AT91C_TC_CLKS_TIMER_DIV3_CLOCK
#elif GYROS_CONFIG_TIMER_DIV == 128
#define TIMER_CLOCK      AT91C_TC_CLKS_TIMER_DIV4_CLOCK
#elif GYROS_CONFIG_TIMER_DIV == 1024
#define TIMER_CLOCK      AT91C_TC_CLKS_TIMER_DIV5_CLOCK
#else
#error Unsupported GYROS_CONFIG_TIMER_DIV
#endif

static gyros_abstime_t s_time_hi;
static unsigned short s_last_time_lo;

static void
tc_isr(void)
{
    if (TC->TC_SR & AT91C_TC_CPCS)
        gyros__tick(gyros_time());
}

void
gyros__dyntick_suspend(void)
{
    /* We can't suspend the tick, because it would make gyros_time()
     * fail, so we set it to the maximum period. */
    TC->TC_RC = s_last_time_lo + MAX_PERIOD;
}

void
gyros__dyntick_set(gyros_abstime_t now, gyros_abstime_t next_timeout)
{
    gyros_reltime_t dt = next_timeout - now;

    if (dt >= MAX_PERIOD)
        TC->TC_RC = s_last_time_lo + MAX_PERIOD;
    else
    {
        TC->TC_RC = next_timeout;

        /* If dt is small enough that CNTR may already be past the
         * value we just set OC1R to, we check if it did indeed miss
         * the value, and in that case force a timer interrupt using
         * OC2 (which is always on). */
        if (dt < MAX_PERIOD / 4 && (short)((unsigned short)next_timeout -
                                           (unsigned short)TC->TC_CV) <= 0)
        {
            TC->TC_RC = TC->TC_CV + 2;
        }
    }
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    unsigned short time_lo = TC->TC_CV;
    gyros_abstime_t time_hi;

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
    gyros_target_aic_init();
    gyros_target_set_isr(GYROS_IRQ_TC0 + GYROS_CONFIG_TIMER_NUM,
                         GYROS_IRQ_MODE_INT_LEVEL, tc_isr);

    /* Enable the timer peripheral */
    AT91C_BASE_PMC->PMC_PCER = 1U << (AT91C_ID_TC0 + GYROS_CONFIG_TIMER_NUM);

    /* Select the desired clock source. */
    TC->TC_CMR = TIMER_CLOCK;

    /* Set up an interrupt at MAX_PERIOD and enable the RC interrupt. */
    TC->TC_RC = MAX_PERIOD;
    TC->TC_IDR = 0xffffffff;
    TC->TC_IER = AT91C_TC_CPCS;

    /* Reset and start the timer */
    TC->TC_CCR = AT91C_TC_CLKEN | AT91C_TC_SWTRG;

    gyros__interrupt_enable();
}
