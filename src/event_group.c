/**************************************************************************
 * Copyright (c) 2002-2018, Tobias Ringström <tobias@ringis.se>
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
#include <gyros/event_group.h>
#include <gyros/interrupt.h>
#include <gyros/private/trace.h>

#include <stddef.h>

#include "private.h"

static bool
event_match(bool wait_all, unsigned wait_events, unsigned events)
{
    return wait_all ? ((events & wait_events) == wait_events)
                    : ((events & wait_events) != 0);
}

void
gyros_event_group_init(gyros_event_group_t *e)
{
    GYROS_DEBUG_INFO_INIT(e, GYROS_EVENT_GROUP_DEBUG_MAGIC);
    e->events = 0;
    GYROS__LIST_NODE_INIT(&e->task_list);
}

void
gyros_event_group_get(gyros_event_group_t *e,
                      unsigned wait_events,
                      bool wait_all,
                      unsigned *events)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (e->debug_info.magic != GYROS_EVENT_GROUP_DEBUG_MAGIC)
        gyros__error("uninitialized event group in event_group_get", e);
    if (gyros_in_interrupt())
        gyros__error("event_group_get called from interrupt", e);
    if (gyros_interrupts_disabled())
        gyros__error("event_group_get called with interrupts disabled", e);
#endif

    flags = gyros_interrupt_disable();

    while (GYROS_UNLIKELY(!event_match(wait_all, wait_events, e->events)))
    {
        GYROS__TRACE_EVENT_GROUP(BLOCKED, e);
        gyros__task_move(_gyros.current, &e->task_list);
        GYROS_DEBUG_SET_STATE2(_gyros.current, "event_group_get", e);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
    }
    if (events)
        *events = e->events & wait_events;
    e->events &= ~(e->events & wait_events);
    GYROS__TRACE_EVENT_GROUP(AQUIRED, e);
    gyros_interrupt_restore(flags);
}

bool
gyros_event_group_get_until(gyros_event_group_t *e,
                            unsigned wait_events,
                            bool wait_all,
                            unsigned *events,
                            gyros_abstime_t timeout)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (e->debug_info.magic != GYROS_EVENT_GROUP_DEBUG_MAGIC)
        gyros__error("uninitialized event group in event_group_get_until", e);
    if (gyros_in_interrupt())
        gyros__error("event_group_get_until called from interrupt", e);
    if (gyros_interrupts_disabled())
        gyros__error("event_group_get_until called with interrupts disabled", e);
#endif

    flags = gyros_interrupt_disable();

    while (GYROS_UNLIKELY(!event_match(wait_all, wait_events, e->events)))
    {
        if (!gyros__task_set_timeout(timeout))
        {
            gyros_interrupt_restore(flags);
            return false;
        }
        GYROS__TRACE_EVENT_GROUP(BLOCKED, e);
        gyros__task_move(_gyros.current, &e->task_list);
        GYROS_DEBUG_SET_STATE2(_gyros.current, "event_group_get_until", e);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
        if (!event_match(wait_all, wait_events, e->events))
        {
            gyros_interrupt_restore(flags);
            return false;
        }
    }
    if (events)
        *events = e->events & wait_events;
    e->events &= ~(e->events & wait_events);
    GYROS__TRACE_EVENT_GROUP(AQUIRED, e);
    gyros_interrupt_restore(flags);

    return true;
}

void
gyros_event_group_set(gyros_event_group_t *e, unsigned events)
{
    if (events == 0)
        return;

    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (e->debug_info.magic != GYROS_EVENT_GROUP_DEBUG_MAGIC)
        gyros__error("uninitialized event group in event_group_set", e);
#endif

    GYROS__TRACE_EVENT_GROUP(SET, e);
    if (GYROS_LIKELY((events & ~e->events) != 0))
    {
        e->events |= events;
        /* Move the tasks in reverse order to preserve the order of
         * the tasks (of equal priority) in the list. */
        while (!gyros__list_empty(&e->task_list))
            gyros__task_wake(TASK(e->task_list.prev));
        gyros__cond_reschedule();
    }
    gyros_interrupt_restore(flags);
}

void
gyros_event_group_clear(gyros_event_group_t *e, unsigned events)
{
    if (events == 0)
        return;

    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (e->debug_info.magic != GYROS_EVENT_GROUP_DEBUG_MAGIC)
        gyros__error("uninitialized event group in event_group_clear", e);
#endif

    GYROS__TRACE_EVENT_GROUP(CLEAR, e);
    e->events &= ~events;
    gyros_interrupt_restore(flags);
}
