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
#include <gyros/target/config.h>
#include <gyros/time.h>

#include <gyros/arch/armv7-m/nvic.h>

#if !GYROS_CONFIG_DYNTICK
static gyros_abstime_t s_time;

static void
systick_handler(void)
{
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
#endif

void
gyros__arch_init(void)
{
#if !GYROS_CONFIG_DYNTICK
    unsigned long *vtab = (unsigned long*)NVIC_VTABOFFSET;

    vtab[15] = (unsigned long)systick_handler;

    NVIC_SYSTICK_RELOAD = GYROS_CONFIG_CORE_HZ /
                          GYROS_CONFIG_TIMER_RESOLUTION - 1;
    NVIC_SYSTICK_CURRENT = 0; /* reset on write register */
    NVIC_SYSTICK_CTLST = (NVIC_SYSTICK_CTLST & ~0x00010007) | 7;
#endif
}

void
gyros__idle(void)
{
}
