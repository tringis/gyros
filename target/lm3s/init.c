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
#include <gyros/arch/armv7-m/target.h>

#include "../../arch/armv7-m/nvic.h"

#if GYROS_CONFIG_DYNTICK
#define TIMER_IRQ  (GYROS_CONFIG_LM3S_TIMER == 0 ? LM3S_IRQ_TIMER_0A : \
                    GYROS_CONFIG_LM3S_TIMER == 1 ? LM3S_IRQ_TIMER_1A : \
                    GYROS_CONFIG_LM3S_TIMER == 2 ? LM3S_IRQ_TIMER_2A : \
                    LM3S_IRQ_TIMER_3A)

#define MAX_PERIOD           0x80000000

#define REG32(addr)          (*(volatile unsigned long*)(addr))

#define SYSCTL_REG(offset)   REG32(0x400fe000 + (offset))

#define SYSCTL_RCGC1         SYSCTL_REG(0x104)
#define SYSCTL_SCGC1         SYSCTL_REG(0x114)
#define SYSCTL_DCGC1         SYSCTL_REG(0x124)

#define TIMER_BASE           (0x40030000 + \
                              GYROS_CONFIG_LM3S_TIMER * 0x1000)
#define TIMER_REG(offset)    REG32(TIMER_BASE + (offset))

#define GPTMCFG              TIMER_REG(0x000)
#define GPTMTAMR             TIMER_REG(0x004)
#define GPTMTAMR_TACDIR      (1U << 4)
#define GPTMTAMR_TAMIE       (1U << 5)
#define GPTMTAMR_TAMR_PERIODIC (2U << 0)
#define GPTMTBMR             TIMER_REG(0x008)
#define GPTMCTL              TIMER_REG(0x00C)
#define GPTMCTL_TAEN         (1U << 0)
#define GPTMIMR              TIMER_REG(0x018)
#define GPTMIMR_TAMIM        (1U << 4)
#define GPTMRIS              TIMER_REG(0x01C)
#define GPTMMIS              TIMER_REG(0x020)
#define GPTMICR              TIMER_REG(0x024)
#define GPTMICR_TAMCINT      (1U << 4)
#define GPTMTAILR            TIMER_REG(0x028)
#define GPTMTBILR            TIMER_REG(0x02C)
#define GPTMTAMATCHR         TIMER_REG(0x030)
#define GPTMTBMATCHR         TIMER_REG(0x034)
#define GPTMTAPR             TIMER_REG(0x038)
#define GPTMTBPR             TIMER_REG(0x03C)
#define GPTMTAR              TIMER_REG(0x048)
#define GPTMTBR              TIMER_REG(0x04C)
#define GPTMTAV              TIMER_REG(0x050)
#define GPTMTBV              TIMER_REG(0x054)

static gyros_abstime_t s_time_hi;
static unsigned long s_last_time_lo;

static void
timer_isr(void)
{
    GPTMICR = GPTMICR_TAMCINT; /* Clear match interrupt */
    gyros__tick(gyros_time());
}

void
gyros__dyntick_suspend(void)
{
    /* We can't suspend the tick, because it would make gyros_time()
     * fail, so we set it to the maximum period. */
    GPTMTAMATCHR = s_last_time_lo + MAX_PERIOD;
}

void
gyros__dyntick_set(gyros_abstime_t now, gyros_abstime_t next_timeout)
{
    gyros_reltime_t dt = next_timeout - now;

    if (dt >= MAX_PERIOD)
        GPTMTAMATCHR = s_last_time_lo + MAX_PERIOD;
    else
    {
        GPTMTAMATCHR = next_timeout;

        /* If dt is small enough to be a risk, we check if the time is
         * already past, and if so just pend the timer interrupt. */
        if (dt < MAX_PERIOD / 4 &&
            (long)((unsigned long)next_timeout - GPTMTAV) <= 0)
        {
            NVIC_IRQ_SET_PENDING(TIMER_IRQ);
        }
    }
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    unsigned long time_lo = GPTMTAV;
    gyros_abstime_t time_hi;

    if (time_lo < s_last_time_lo)
        ++s_time_hi;
    s_last_time_lo = time_lo;
    time_hi = s_time_hi;

    gyros_interrupt_restore(flags);

    return (time_hi << 32) | time_lo;
}
#endif

void
gyros__target_init(void)
{
    gyros__arch_init();

#if GYROS_CONFIG_DYNTICK
    /* Enable timer in all power modes */
    SYSCTL_RCGC1 |= 1U << (16 + GYROS_CONFIG_LM3S_TIMER);
    SYSCTL_SCGC1 |= 1U << (16 + GYROS_CONFIG_LM3S_TIMER);
    SYSCTL_DCGC1 |= 1U << (16 + GYROS_CONFIG_LM3S_TIMER);
    __asm__ __volatile__("dmb\n" ::: "memory");

    GPTMCTL &= 0xffff9080; /* Disable timer */
    GPTMCFG = GPTMCFG & ~7; /* 32 bit mode */
    GPTMTAMR = (GPTMTAMR & ~0xff) |
        GPTMTAMR_TACDIR | GPTMTAMR_TAMR_PERIODIC; /* Count up, periodic */
    GPTMIMR = (GPTMIMR & 0xfffff0e0) | GPTMIMR_TAMIM; /* Match interrupt */
    GPTMTAILR = 0xffffffff; /* Count from zero */

    gyros_target_set_isr(TIMER_IRQ, 0, timer_isr);

    GPTMTAMR |= GPTMTAMR_TAMIE; /* Enable interrupt */
    GPTMCTL |= GPTMCTL_TAEN; /* Enable timer */

    gyros__dyntick_suspend();
#endif
}
