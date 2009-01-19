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

#include <stddef.h>

#include "private.h"

#if GYROS_CONFIG_WAIT
gyros_task_t*
gyros_task_wait(void)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_task_t *task;

#if GYROS_CONFIG_DEBUG
    if (gyros_in_interrupt())
        gyros_error("task_wait called from interrupt");
#endif

    while (gyros__zombies.next == &gyros__zombies)
    {
        gyros__task_move(gyros__state.current, &gyros__reapers);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "task_wait";
        gyros__state.current->debug_object = NULL;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
    }
    task = TASK(gyros__zombies.next);
    gyros__list_remove(&task->main_list);
#if GYROS_CONFIG_DEBUG
    task->debug_magic = 0;
#endif
    gyros_interrupt_restore(flags);

    return task;
}

gyros_task_t*
gyros_task_timedwait(gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_task_t *task;

#if GYROS_CONFIG_DEBUG
    if (gyros_in_interrupt())
        gyros_error("task_timedwait called from interrupt");
#endif

    if (gyros__list_empty(&gyros__zombies))
    {
        gyros__task_move(gyros__state.current, &gyros__reapers);
        gyros__task_set_timeout(timeout);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "task_timedwait";
        gyros__state.current->debug_object = NULL;
#endif
        gyros__cond_reschedule();
        if (gyros__list_empty(&gyros__zombies))
        {
            gyros_interrupt_restore(flags);
            return NULL;
        }
    }

    task = TASK(gyros__zombies.next);
    gyros__list_remove(&task->main_list);
#if GYROS_CONFIG_DEBUG
    task->debug_magic = 0;
#endif
    gyros_interrupt_restore(flags);

    return task;
}
#endif
