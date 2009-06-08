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
#include <gyros/cond.h>

#include <stddef.h>

#include "private.h"

void
gyros_cond_init(gyros_cond_t *c)
{
#if GYROS_CONFIG_DEBUG
    c->debug_magic = GYROS_COND_DEBUG_MAGIC;
    c->name = NULL;
#endif

    GYROS__LIST_NODE_INIT(&c->task_list);
}

void
gyros_cond_name(gyros_cond_t *c, const char *name)
{
#if GYROS_CONFIG_DEBUG
    c->name = name;
#endif
}

void
gyros_cond_wait(gyros_cond_t *c, gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (c->debug_magic != GYROS_COND_DEBUG_MAGIC)
        gyros_error("uninitialized cond in cond_wait", c);
    if (gyros_in_interrupt())
        gyros_error("cond_wait called from interrupt", c);
#endif

    GYROS__TRACE_COND(WAIT, c);
    gyros__mutex_unlock(m, 0);
    gyros__task_move(gyros.current, &c->task_list);
#if GYROS_CONFIG_DEBUG
    gyros.current->debug_state = "cond_wait";
    gyros.current->debug_object = c;
#endif
    gyros_interrupt_restore(flags);

    gyros__cond_reschedule();

    gyros_mutex_lock(m);
}

int
gyros_cond_wait_until(gyros_cond_t *c, gyros_mutex_t *m,
                      gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (c->debug_magic != GYROS_COND_DEBUG_MAGIC)
        gyros_error("uninitialized cond in cond_wait_until", c);
    if (gyros_in_interrupt())
        gyros_error("cond_wait_until called from interrupt", c);
#endif

    GYROS__TRACE_COND(WAIT, c);
    gyros__mutex_unlock(m, 0);
    gyros__task_move(gyros.current, &c->task_list);
    gyros__task_set_timeout(timeout);
#if GYROS_CONFIG_DEBUG
    gyros.current->debug_state = "cond_wait_until";
    gyros.current->debug_object = c;
#endif
    gyros_interrupt_restore(flags);
    gyros__cond_reschedule();

    gyros_mutex_lock(m);

    return !gyros.current->timed_out;
}

void
gyros_cond_signal_one(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (c->debug_magic != GYROS_COND_DEBUG_MAGIC)
        gyros_error("uninitialized cond in cond_signal_one", c);
#endif

    GYROS__TRACE_COND(SIGNAL_ONE, c);
    if (gyros__list_empty(&c->task_list))
        gyros_interrupt_restore(flags);
    else
    {
        gyros__task_wake(TASK(c->task_list.next));
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
    }
}

void
gyros_cond_signal_all(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (c->debug_magic != GYROS_COND_DEBUG_MAGIC)
        gyros_error("uninitialized cond in signal_all", c);
#endif

    GYROS__TRACE_COND(SIGNAL_ALL, c);
    if (gyros__list_empty(&c->task_list))
        gyros_interrupt_restore(flags);
    else
    {
        /* Move the tasks in reverse order to preserve the order of
         * the tasks (of equal priority) in the list. */
        while (!gyros__list_empty(&c->task_list))
            gyros__task_wake(TASK(c->task_list.prev));
        gyros_interrupt_restore(flags);
        if (!gyros_in_interrupt())
            gyros__cond_reschedule();
    }
}
