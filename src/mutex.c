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
#include <gyros/mutex.h>

#include <stddef.h>

#include "private.h"

void
gyros_mutex_init(gyros_mutex_t *m)
{
#if GYROS_CONFIG_DEBUG
    m->debug_magic = GYROS_MUTEX_DEBUG_MAGIC;
#endif

    m->owner = NULL;
    GYROS__LIST_NODE_INIT(&m->task_list);
}

int
gyros_mutex_try_lock(gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (m->debug_magic != GYROS_MUTEX_DEBUG_MAGIC)
        gyros_error("uninitialized mutex in mutex_try_lock", m);
    if (gyros_in_interrupt())
        gyros_error("mutex_try_lock called from interrupt", m);
#endif

    GYROS__TRACE_MUTEX(LOCK, m);
    if (unlikely(m->owner != NULL))
    {
        gyros_interrupt_restore(flags);
        return 0;
    }

    m->owner = gyros__state.current;
    m->owner_priority = gyros__state.current->priority;
    GYROS__TRACE_MUTEX(AQUIRED, m);
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_mutex_lock(gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (m->debug_magic != GYROS_MUTEX_DEBUG_MAGIC)
        gyros_error("uninitialized mutex in mutex_lock", m);
    if (gyros_in_interrupt())
        gyros_error("mutex_lock called from interrupt", m);
    if (m->owner == gyros__state.current)
        gyros_error("mutex_lock deadlock", m);
#endif

    GYROS__TRACE_MUTEX(LOCK, m);
    while (unlikely(m->owner != NULL))
    {
        gyros__task_move(gyros__state.current, &m->task_list);
        /* Implement priority inheritance to prevent priority
         * inversion. */
        if (m->owner->priority < gyros__state.current->priority)
        {
            m->owner->raised_priority = 1;
            m->owner->priority = gyros__state.current->priority;
        }
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "mutex_lock";
        gyros__state.current->debug_object = m;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
    }

    m->owner = gyros__state.current;
    m->owner_priority = gyros__state.current->priority;
    GYROS__TRACE_MUTEX(AQUIRED, m);
    gyros_interrupt_restore(flags);
}

void
gyros__mutex_unlock(gyros_mutex_t *m, int reschedule)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (m->debug_magic != GYROS_MUTEX_DEBUG_MAGIC)
        gyros_error("uninitialized mutex in mutex__unlock", m);
    if (m->owner == NULL)
        gyros_error("mutex__unlock called for unlocked mutex", m);
    if (m->owner != gyros__state.current)
        gyros_error("mutex__unlock called by non owner task", m);
#endif

    m->owner = NULL;
    if (unlikely(gyros__state.current->raised_priority))
    {
        gyros__state.current->raised_priority = 0;
        gyros__state.current->priority = m->owner_priority;
    }
    GYROS__TRACE_MUTEX(UNLOCK, m);
    if (likely(gyros__list_empty(&m->task_list)))
        gyros_interrupt_restore(flags);
    else
    {
        gyros__task_wake(TASK(m->task_list.next));
        gyros_interrupt_restore(flags);
        if (reschedule)
            gyros__cond_reschedule();
    }
}

void
gyros_mutex_unlock(gyros_mutex_t *m)
{
    gyros__mutex_unlock(m, 1);
}
