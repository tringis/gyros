/**************************************************************************
 * Copyright (c) 2002-2008, Tobias Ringström <tobias@ringis.se>
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
#include <gyros/cond.h>

#include "private.h"

void
gyros_cond_init(gyros_cond_t *c)
{
    GYROS_LIST_NODE_INIT(&c->task_list);
}

void
gyros_cond_wait(gyros_cond_t *c, gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__mutex_unlock(m, 0);
    gyros__task_move(gyros__state.current, &c->task_list);
    gyros_interrupt_restore(flags);

    gyros__reschedule();

    gyros_mutex_lock(m);
}

int
gyros_cond_timedwait(gyros_cond_t *c, gyros_mutex_t *m, int timeout)
{
    unsigned long flags = gyros_interrupt_disable();
    int timed_out;

    gyros__mutex_unlock(m, 0);
    gyros__task_move(gyros__state.current, &c->task_list);
    timed_out = gyros__task_timeout(timeout);
    gyros_interrupt_restore(flags);

    gyros_mutex_lock(m);

    return timed_out;
}

void
gyros_cond_signal_one(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

    if (!gyros_list_empty(&c->task_list))
    {
        gyros__task_wake(TASK(c->task_list.next));
        gyros__cond_reschedule();
    }
    gyros_interrupt_restore(flags);
}

void
gyros_cond_signal_all(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

    if (!gyros_list_empty(&c->task_list))
    {
        /* Move the tasks in reverse order to preserve the order of
         * the tasks (of equal priority) in the list. */
        while (!gyros_list_empty(&c->task_list))
            gyros__task_wake(TASK(c->task_list.prev));
        gyros__cond_reschedule();
    }
    gyros_interrupt_restore(flags);
}
