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
#include <gyros/task.h>

#include "private.h"

#if GYROS_CONFIG_DYNTICK
void
gyros__dyntick_update(gyros_abstime_t now)
{
#if GYROS_CONFIG_TIMER
    if (gyros__list_empty(&_gyros.timeouts))
    {
        if (gyros__list_empty(&_gyros.timers))
            gyros__dyntick_suspend();
        else
            gyros__dyntick_set(now, TIMER(_gyros.timers.next)->timeout);
    }
    else
    {
        if (gyros__list_empty(&_gyros.timers) ||
            (gyros_reltime_t)(TIMEOUT(_gyros.timeouts.next)->timeout -
                              TIMER(_gyros.timers.next)->timeout) < 0)
        {
            gyros__dyntick_set(now, TIMEOUT(_gyros.timeouts.next)->timeout);
        }
        else
            gyros__dyntick_set(now, TIMER(_gyros.timers.next)->timeout);
    }
#else
    if (gyros__list_empty(&_gyros.timeouts))
        gyros__dyntick_suspend();
    else
        gyros__dyntick_set(now, TIMEOUT(_gyros.timeouts.next)->timeout);
#endif
}
#endif

int
gyros__task_set_timeout(gyros_abstime_t timeout)
{
    gyros_abstime_t now = gyros_time();
    if (gyros_time_compare(timeout, now) <= 0)
        return 0;

    struct gyros__list_node *i;
    for (i = _gyros.timeouts.next; i != &_gyros.timeouts; i = i->next)
    {
        if ((gyros_reltime_t)(timeout - TIMEOUT(i)->timeout) < 0)
            break;
    }
    gyros__list_insert_before(&_gyros.current->timeout_list_node, i);
    _gyros.current->timeout = timeout;
    _gyros.current->timed_out = 0;
    gyros__dyntick_update(now);
    return 1;
}

void
gyros__tick(gyros_abstime_t now)
{
    unsigned long flags = gyros_interrupt_disable();
    int reschedule = 0;

    while (!gyros__list_empty(&_gyros.timeouts))
    {
        gyros_task_t *task = TIMEOUT(_gyros.timeouts.next);

        if ((gyros_reltime_t)(now - task->timeout) < 0)
            break;

        task->timed_out = 1;
        gyros__task_wake(task);
        reschedule = 1;
    }

    if (reschedule)
        gyros__tick_reschedule();

#if GYROS_CONFIG_TIMER
    while (!gyros__list_empty(&_gyros.timers))
    {
        gyros_timer_t *timer = TIMER(_gyros.timers.next);

        if ((gyros_reltime_t)(now - timer->timeout) < 0)
            break;

        gyros__list_remove(&timer->list_node);
        if (timer->period)
        {
            timer->timeout += timer->period;
            gyros__timer_schedule(timer);
        }
        GYROS__TRACE_TIMER(CALLBACK, timer);
        timer->callback(timer, now, timer->callback_arg);
    }
#endif

    gyros__dyntick_update(now);
    gyros_interrupt_restore(flags);
}
