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
#include <gyros/compiler.h>
#include <gyros/interrupt.h>
#include <gyros/private/trace.h>
#include <gyros/mutex.h>

#include <stddef.h>

#include "private.h"

void
gyros_mutex_init(gyros_mutex_t *m)
{
    GYROS_DEBUG_INFO_INIT(m, GYROS_MUTEX_DEBUG_MAGIC);
    m->owner = NULL;
    GYROS__LIST_NODE_INIT(&m->task_list);
}

int
gyros__mutex_try_lock_slow(gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (m->debug_info.magic != GYROS_MUTEX_DEBUG_MAGIC)
        gyros__error("uninitialized mutex in mutex_try_lock", m);
    if (gyros_in_interrupt())
        gyros__error("mutex_try_lock called from interrupt", m);
#endif

    if (GYROS_UNLIKELY(m->owner != NULL))
    {
        gyros_interrupt_restore(flags);
        return 0;
    }

    m->owner = gyros.current;
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros__mutex_lock_slow(gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (m->debug_info.magic != GYROS_MUTEX_DEBUG_MAGIC)
        gyros__error("uninitialized mutex in mutex_lock", m);
    if (gyros_in_interrupt())
        gyros__error("mutex_lock called from interrupt", m);
    if (m->owner == gyros.current)
        gyros__error("mutex_lock deadlock", m);
#endif

    if (GYROS_UNLIKELY(m->owner != NULL))
    {
        GYROS__TRACE_MUTEX(BLOCKED, m);
        do
        {
            gyros__task_move(gyros.current, &m->task_list);
            /* Implement priority inheritance to prevent priority
             * inversion. */
            if (m->owner->priority < gyros.current->priority)
            {
                m->owner->priority = gyros.current->priority;
                gyros__task_move(m->owner, m->owner->main_list);
            }
            GYROS_DEBUG_SET_STATE2(gyros.current, "mutex_lock", m);
            gyros__reschedule();
            gyros_interrupt_restore(flags);
            flags = gyros_interrupt_disable();
        } while (GYROS_UNLIKELY(m->owner != NULL));
        GYROS__TRACE_MUTEX(AQUIRED, m);
    }

    m->owner = gyros.current;
    gyros_interrupt_restore(flags);
}

void
gyros__mutex_unlock_slow(gyros_mutex_t *m, int reschedule)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (m->debug_info.magic != GYROS_MUTEX_DEBUG_MAGIC)
        gyros__error("uninitialized mutex in mutex__unlock", m);
    if (m->owner == NULL)
        gyros__error("mutex__unlock called for unlocked mutex", m);
    if (m->owner != gyros.current)
        gyros__error("mutex__unlock called by non owner task", m);
#endif

    m->owner = NULL;
    if (GYROS_UNLIKELY(gyros.current->priority != gyros.current->base_priority))
    {
        gyros.current->priority = gyros.current->base_priority;
        gyros__task_move(gyros.current, gyros.current->main_list);
    }
    if (GYROS_LIKELY(gyros__list_empty(&m->task_list)))
        gyros_interrupt_restore(flags);
    else
    {
        GYROS__TRACE_MUTEX(UNLOCK, m);
        gyros__task_wake(TASK(m->task_list.next));
        if (reschedule)
            gyros__cond_reschedule();
        gyros_interrupt_restore(flags);
    }
}
