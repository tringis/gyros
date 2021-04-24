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

void
gyros_task_wait(gyros_task_t *task)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (gyros_in_interrupt())
        gyros__error("task_wait called from interrupt", NULL);
    if (gyros_interrupts_disabled())
        gyros__error("task_wait called with interrupts disabled", NULL);
#endif

    flags = gyros_interrupt_disable();

    while (!task->finished)
    {
        gyros__task_move(gyros.current, &task->waiter_list);
        GYROS_DEBUG_SET_STATE(gyros.current, "task_wait");
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
    }
    gyros_interrupt_restore(flags);
}

bool
gyros_task_wait_until(gyros_task_t *task, gyros_abstime_t timeout)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (gyros_in_interrupt())
        gyros__error("task_wait_until called from interrupt", NULL);
    if (gyros_interrupts_disabled())
        gyros__error("task_wait_until called with interrupts disabled", NULL);
#endif

    flags = gyros_interrupt_disable();

    if (!gyros__task_set_timeout(timeout))
    {
        gyros_interrupt_restore(flags);
        return false;
    }

    gyros__task_move(gyros.current, &task->waiter_list);
    GYROS_DEBUG_SET_STATE(gyros.current, "task_wait_until");
    gyros__reschedule();
    gyros_interrupt_restore(flags);

    return task->finished;
}
