/**************************************************************************
 * Copyright (c) 2002-2022, Tobias Ringström <tobias@ringis.se>
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
#include <gyros/compiler.h>
#include <gyros/interrupt.h>
#include <gyros/private/trace.h>
#include <gyros/alarm_clock.h>

#include <stddef.h>

#include "private.h"

void
gyros_alarm_clock_init(gyros_alarm_clock_t *a)
{
    GYROS_DEBUG_INFO_INIT(a, GYROS_ALARM_CLOCK_DEBUG_MAGIC);
    a->armed = false;
    GYROS__LIST_NODE_INIT(&a->task_list);
}

void
gyros_alarm_clock_wait(gyros_alarm_clock_t *a)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (a->debug_info.magic != GYROS_ALARM_CLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized alarm_clock in alarm_clock_wait", a);
    if (gyros_in_interrupt())
        gyros__error("alarm_clock_wait called from interrupt", a);
    if (gyros_interrupts_disabled())
        gyros__error("alarm_clock_wait called with interrupts disabled", a);
#endif

    flags = gyros_interrupt_disable();

    if (!a->armed || gyros__task_set_timeout(a->wakeup_time))
    {
        GYROS__TRACE_ALARM_CLOCK(WAIT, a);
        gyros__task_move(gyros.current, &a->task_list);
        GYROS_DEBUG_SET_STATE2(gyros.current, "alarm_clock_wait", a);
        gyros__reschedule();
    }
    gyros_interrupt_restore(flags);
}

void
gyros_alarm_clock_set(gyros_alarm_clock_t *a, gyros_abstime_t wakeup_time)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (a->debug_info.magic != GYROS_ALARM_CLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized alarm_clock in alarm_clock_signal", a);
#endif

    a->armed = true;
    a->wakeup_time = wakeup_time;
    GYROS__TRACE_ALARM_CLOCK(SET, a);

    if (!gyros__list_empty(&a->task_list))
    {
        gyros_abstime_t now = gyros_time();
        if (gyros_time_compare(wakeup_time, now) <= 0)
        {
            while (!gyros__list_empty(&a->task_list))
                gyros__task_wake(TASK(a->task_list.next));
            gyros__cond_reschedule();
        }
        else
        {
            struct gyros__list_node *ai;
            // Remove all tasks from the timeout list...
            for (ai = a->task_list.next; ai != &a->task_list; ai = ai->next)
                gyros__list_remove(&TASK(ai)->timeout_list_node);
            // ...find where to put the tasks...
            struct gyros__list_node *ti;
            for (ti = gyros.timeouts.next; ti != &gyros.timeouts; ti = ti->next)
            {
                if ((gyros_reltime_t)(wakeup_time - TIMEOUT(ti)->timeout) < 0)
                    break;
            }
            // ...and add them
            for (ai = a->task_list.prev; ai != &a->task_list; ai = ai->prev)
            {
                gyros__list_insert_before(&TASK(ai)->timeout_list_node, ti);
                TASK(ai)->timeout = wakeup_time;
                TASK(ai)->timed_out = 0;
            }
            gyros__dyntick_update(now);
        }
    }
    gyros_interrupt_restore(flags);
}

void
gyros_alarm_clock_reset(gyros_alarm_clock_t *a)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (a->debug_info.magic != GYROS_ALARM_CLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized alarm_clock in alarm_clock_signal", a);
#endif

    GYROS__TRACE_ALARM_CLOCK(RESET, a);
    a->armed = false;
    gyros_interrupt_restore(flags);
}
