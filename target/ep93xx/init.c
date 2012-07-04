/**************************************************************************
 * Copyright (c) 2002-2012, Tobias Ringström <tobias@ringis.se>
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
#include <gyros/config.h>
#include <gyros/time.h>

#include "ep93xx.h"

static gyros_abstime_t s_time;

static void
tim_isr(void)
{
    /* Clear the TIM1 interrupt. */
    TIM1_Clear = 0;
    gyros__tick(++s_time);
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
    TIM1_Cntl = 0; /* Disabled */
    TIM1_Clear = 0; /* Clear counter interrupt */

    gyros_target_set_isr(GYROS_IRQ_TC1UI, tim_isr);

    /* Calculate cycle time and round up so that any timeout specified
     * by the user will be the same or later. */
    TIM1_Load = ((5084689 + 10 * GYROS_CONFIG_HZ - 1) /
                 (10 * GYROS_CONFIG_HZ)) - 1;

    TIM1_Cntl = 0xc8; /* Enabled + periodic + 508468.9 Hz clock */

    gyros__interrupt_enable();
}
