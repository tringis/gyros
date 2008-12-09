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
#include <gyros/task.h>

#include <limits.h>

#include "private.h"

#define TIMEOUT_TASK(t) GYROS_LIST_CONTAINER(t, gyros_task_t, timeout_list)

static struct gyros_list_node s_sleeping = { &s_sleeping, &s_sleeping };

void
gyros__task_set_timeout(gyros_abstime_t timeout)
{
    struct gyros_list_node *i;

    for (i = s_sleeping.next; i != &s_sleeping; i = i->next)
    {
        if ((gyros_time_t)(timeout - TIMEOUT_TASK(i)->timeout) < 0)
            break;
    }
    gyros_list_insert_before(&gyros__state.current->timeout_list, i);
    gyros__state.current->timeout = timeout;
    gyros__state.current->timed_out = 0;
    if (s_sleeping.next == &gyros__state.current->timeout_list)
        gyros__update_tick(timeout);
}

void
gyros__wake_sleeping_tasks(void)
{
    gyros_abstime_t now = gyros_time();

    while (!gyros_list_empty(&s_sleeping) &&
           (gyros_time_t)(now - TIMEOUT_TASK(s_sleeping.next)->timeout) >= 0)
    {
        gyros_task_t *task = TIMEOUT_TASK(s_sleeping.next);

        task->timed_out = 1;
        gyros__task_wake(task);
    }

    if (gyros_list_empty(&s_sleeping))
        gyros__suspend_tick();
    else
        gyros__update_tick(TIMEOUT_TASK(s_sleeping.next)->timeout);
}

int
gyros_sleep_until(gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros_list_remove(&gyros__state.current->main_list);
    gyros__task_set_timeout(timeout);
    gyros_interrupt_restore(flags);
    gyros__cond_reschedule();

    return gyros__state.current->timed_out;
}
