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

#include "private.h"

#define TASK_LIST_TASK(t) GYROS_LIST_CONTAINER(t, gyros_task_t, task_list)

static gyros_task_t*
find_zombie(gyros_task_t *task)
{
    struct gyros_list_node *i;

    for (i = gyros__zombies.next; i != &gyros__zombies; i = i->next)
    {
        if (!task || task == TASK_LIST_TASK(i))
            return TASK_LIST_TASK(i);
    }

    return 0;
}

gyros_task_t*
gyros_task_wait(gyros_task_t *task)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_task_t *t;

    while ((t = find_zombie(task)) == 0)
    {
        gyros__task_move(gyros__state.current, &gyros__reapers);
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
    }
    gyros_list_remove(&t->task_list);
    gyros_interrupt_restore(flags);

    return t;
}

gyros_task_t*
gyros_task_timedwait(gyros_task_t *task, int timeout)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_task_t *t = find_zombie(task);

    if (!t)
    {
        gyros__task_move(gyros__state.current, &gyros__reapers);
        gyros__task_set_timeout(timeout);
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
        t = find_zombie(task);
    }
    if (t)
        gyros_list_remove(&t->task_list);
    gyros_interrupt_restore(flags);

    return t;
}
