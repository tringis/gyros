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
#include <gyros/sem.h>

#include <limits.h>

#include "private.h"

void
gyros_sem_init(gyros_sem_t *s, int start_value)
{
    s->value = start_value;
    s->max_value = UINT_MAX;
    GYROS_LIST_NODE_INIT(&s->task_list);
}

void
gyros_sem_init_binary(gyros_sem_t *s)
{
    s->value = 0;
    s->max_value = 1;
    GYROS_LIST_NODE_INIT(&s->task_list);
}

void
gyros_sem_wait(gyros_sem_t *s)
{
    unsigned long flags = gyros_interrupt_disable();

    while (s->value == 0)
    {
        gyros__task_move(gyros__state.current, &s->task_list);
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
    }
    s->value--;
    gyros_interrupt_restore(flags);
}

int
gyros_sem_timedwait(gyros_sem_t *s, unsigned long timeout)
{
    unsigned long flags = gyros_interrupt_disable();

    if (s->value == 0)
    {
        gyros__task_move(gyros__state.current, &s->task_list);
        gyros__task_set_timeout(timeout);
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
        if (s->value == 0)
        {
            gyros_interrupt_restore(flags);
            return 0;
        }
    }
    s->value--;
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_sem_signal(gyros_sem_t *s)
{
    unsigned long flags = gyros_interrupt_disable();

    if (s->value >= s->max_value)
        gyros_interrupt_restore(flags);
    else
    {
        s->value++;
        if (gyros_list_empty(&s->task_list))
            gyros_interrupt_restore(flags);
        else
        {
            gyros__task_wake(TASK(s->task_list.next));
            gyros_interrupt_restore(flags);
            gyros__cond_reschedule();
        }
    }
}
