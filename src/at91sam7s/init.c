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
#include <gyros/at91sam7s/interrupt.h>

#include "../private.h"
#include "at91sam7s.h"
#include "private.h"

volatile gyros_abstime_t s_time;

static void
pit_isr(void)
{
    if (AT91C_BASE_PITC->PITC_PISR & AT91C_SYSC_PITS)
    {
        uint32_t status = AT91C_BASE_PITC->PITC_PIVR;

        s_time += status >> 20;
        gyros__wake_timedout_tasks(gyros_time());
    }
}

void
gyros__suspend_tick(void)
{
}

void
gyros__update_tick(gyros_abstime_t next_timeout)
{
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_abstime_t time = s_time;

    gyros_interrupt_restore(flags);

    return time;
}

void
gyros__target_init(void)
{
    uint32_t dummy;

    gyros_target_aic_init();
    gyros_target_add_sys_isr(pit_isr);

    dummy = AT91C_BASE_PITC->PITC_PIVR;
    AT91C_BASE_PITC->PITC_PIMR = AT91C_SYSC_PITEN | AT91C_SYSC_PITIEN |
        (PIT_PERIOD - 1);
}
