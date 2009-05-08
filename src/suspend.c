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
gyros_task_suspend(gyros_task_t *task)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (task->debug_magic != GYROS_TASK_DEBUG_MAGIC)
        gyros_error("resume non-task", task);
#endif

    gyros__list_remove(&task->main_list_node);
    gyros__list_remove(&task->timeout_list_node);
#if GYROS_CONFIG_DEBUG
    task->debug_state = "suspended";
    task->debug_object = NULL;
#endif
    gyros_interrupt_restore(flags);
    if (task == gyros__state.current)
        gyros__cond_reschedule();
}

void
gyros_task_resume(gyros_task_t *task)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (task->debug_magic != GYROS_TASK_DEBUG_MAGIC)
        gyros_error("resume non-task", task);
#endif

    gyros__task_move(task, &gyros__state.running);
    gyros_interrupt_restore(flags);
    if (!gyros_in_interrupt())
        gyros__cond_reschedule();
}
