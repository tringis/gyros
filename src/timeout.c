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
#include <gyros/task.h>

#include <limits.h>
#include <stddef.h>

#include "private.h"

#define TIMEOUT_TASK(t)                                         \
    GYROS__LIST_CONTAINER(t, gyros_task_t, timeout_list_node)

static struct gyros__list_node s_timeouts = GYROS__LIST_INITVAL(s_timeouts);

void
gyros__task_set_timeout(gyros_abstime_t timeout)
{
    struct gyros__list_node *i;

    for (i = s_timeouts.next; i != &s_timeouts; i = i->next)
    {
        if ((gyros_reltime_t)(timeout - TIMEOUT_TASK(i)->timeout) < 0)
            break;
    }
    gyros__list_insert_before(&gyros__state.current->timeout_list_node, i);
    gyros__state.current->timeout = timeout;
    gyros__state.current->timed_out = 0;
#if GYROS_CONFIG_DYNTICK
    if (s_timeouts.next == &gyros__state.current->timeout_list_node)
        gyros__update_tick(gyros_time(), timeout);
#endif
}

void
gyros__wake_timedout_tasks(gyros_abstime_t now)
{
    while (!gyros__list_empty(&s_timeouts) &&
           (gyros_reltime_t)(now - TIMEOUT_TASK(s_timeouts.next)->timeout) >= 0)
    {
        gyros_task_t *task = TIMEOUT_TASK(s_timeouts.next);

        task->timed_out = 1;
        gyros__task_wake(task);
    }

#if GYROS_CONFIG_TIMER
    while (!gyros__list_empty(&gyros__timers) &&
           (gyros_reltime_t)(now - TIMER(gyros__timers.next)->timeout) >= 0)
    {
        gyros_timer_t *timer = TIMER(gyros__timers.next);

        gyros__list_remove(&timer->list_node);
        if (timer->period)
        {
            timer->timeout += timer->period;
            gyros__timer_schedule(timer);
        }
        timer->callback(timer->callback_arg);
    }
#endif

#if GYROS_CONFIG_DYNTICK
#if GYROS_CONFIG_TIMER
    if (gyros__list_empty(&s_timeouts))
    {
        if (gyros__list_empty(&gyros__timers))
            gyros__suspend_tick();
        else
            gyros__update_tick(now, TIMER(gyros__timers.next)->timeout);
    }
    else
    {
        if (gyros__list_empty(&gyros__timers) ||
            (gyros_reltime_t)(TIMEOUT_TASK(s_timeouts.next)->timeout -
                              TIMER(gyros__timers.next)->timeout) < 0)
        {
            gyros__update_tick(now, TIMEOUT_TASK(s_timeouts.next)->timeout);
        }
        else
            gyros__update_tick(now, TIMER(gyros__timers.next)->timeout);
    }
#else
    if (gyros__list_empty(&s_timeouts))
        gyros__suspend_tick();
    else
        gyros__update_tick(now, TIMEOUT_TASK(s_timeouts.next)->timeout);
#endif
#endif
}
