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
#include <gyros/sem.h>

#include <stddef.h>

#include "private.h"

void
gyros_sem_init(gyros_sem_t *s, int start_value)
{
    GYROS_DEBUG_INFO_INIT(s, GYROS_SEM_DEBUG_MAGIC);
    s->value = start_value;
    s->max_value = UINT_MAX;
    GYROS__LIST_NODE_INIT(&s->task_list);
}

void
gyros_sem_init_binary(gyros_sem_t *s)
{
    GYROS_DEBUG_INFO_INIT(s, GYROS_SEM_DEBUG_MAGIC);
    s->value = 0;
    s->max_value = 1;
    GYROS__LIST_NODE_INIT(&s->task_list);
}

void
gyros_sem_wait(gyros_sem_t *s)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (s->debug_info.magic != GYROS_SEM_DEBUG_MAGIC)
        gyros__error("uninitialized sem in sem_wait", s);
    if (gyros_in_interrupt())
        gyros__error("sem_wait called from interrupt", s);
#endif

    while (unlikely(s->value == 0))
    {
        GYROS__TRACE_SEM(BLOCKED, s);
        gyros__task_move(gyros.current, &s->task_list);
        GYROS_DEBUG_SET_STATE2(gyros.current, "sem_wait", s);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
    }
    s->value--;
    GYROS__TRACE_SEM(AQUIRED, s);
    gyros_interrupt_restore(flags);
}

int
gyros_sem_wait_until(gyros_sem_t *s, gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (s->debug_info.magic != GYROS_SEM_DEBUG_MAGIC)
        gyros__error("uninitialized sem in sem_wait_until", s);
    if (gyros_in_interrupt())
        gyros__error("sem_wait_until called from interrupt", s);
#endif

    if (unlikely(s->value == 0))
    {
        GYROS__TRACE_SEM(BLOCKED, s);
        gyros__task_move(gyros.current, &s->task_list);
        gyros__task_set_timeout(timeout);
        GYROS_DEBUG_SET_STATE2(gyros.current, "sem_wait_until", s);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
        if (s->value == 0)
        {
            gyros_interrupt_restore(flags);
            return 0;
        }
    }
    s->value--;
    GYROS__TRACE_SEM(AQUIRED, s);
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_sem_signal(gyros_sem_t *s)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (s->debug_info.magic != GYROS_SEM_DEBUG_MAGIC)
        gyros__error("uninitialized sem in sem_signal", s);
#endif

    GYROS__TRACE_SEM(SIGNAL, s);
    if (unlikely(s->value >= s->max_value))
        gyros_interrupt_restore(flags);
    else
    {
        s->value++;
        if (gyros__list_empty(&s->task_list))
            gyros_interrupt_restore(flags);
        else
        {
            gyros__task_wake(TASK(s->task_list.next));
            gyros__cond_reschedule();
            gyros_interrupt_restore(flags);
        }
    }
}
