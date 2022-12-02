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
#include <gyros/private/trace.h>
#include <gyros/timer.h>

#include <stddef.h>

#include "private.h"

#if GYROS_CONFIG_TIMER

void
gyros_timer_init(gyros_timer_t *timer,
                 void (*callback)(struct gyros_timer *timer,
                                  gyros_abstime_t now,
                                  void *arg),
                 void *callback_arg)
{
    GYROS_DEBUG_INFO_INIT(timer, GYROS_TIMER_DEBUG_MAGIC);

    GYROS__LIST_NODE_INIT(&timer->list_node);
    timer->callback = callback;
    timer->callback_arg = callback_arg;
}

void
gyros__timer_schedule(gyros_timer_t *timer)
{
    struct gyros__list_node *i;

    for (i = _gyros.timers.next; i != &_gyros.timers; i = i->next)
    {
        if ((gyros_reltime_t)(timer->timeout - TIMER(i)->timeout) < 0)
            break;
    }
    gyros__list_insert_before(&timer->list_node, i);
}

void
gyros_timer_start(gyros_timer_t *timer,
                  gyros_abstime_t time,
                  gyros_reltime_t period)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (timer->debug_info.magic != GYROS_TIMER_DEBUG_MAGIC)
        gyros__error("uninitialized timer in gyros_timer_start", timer);
#endif

    gyros__list_remove(&timer->list_node);
    timer->timeout = time;
    timer->period = period;
    GYROS__TRACE_TIMER_START(timer);
    gyros__timer_schedule(timer);
    gyros__dyntick_update(gyros_time());
    gyros_interrupt_restore(flags);
}

void
gyros_timer_stop(gyros_timer_t *timer)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (timer->debug_info.magic != GYROS_TIMER_DEBUG_MAGIC)
        gyros__error("uninitialized timer in gyros_timer_stop", timer);
#endif

    gyros__list_remove(&timer->list_node);
    GYROS__TRACE_TIMER(STOP, timer);
    gyros_interrupt_restore(flags);
}

#endif
