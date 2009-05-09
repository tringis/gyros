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
#include <gyros/rwlock.h>

#include <stddef.h>

#include "private.h"

void
gyros_rwlock_init(gyros_rwlock_t *rwlock)
{
#if GYROS_CONFIG_DEBUG
    rwlock->debug_magic = GYROS_RWLOCK_DEBUG_MAGIC;
    rwlock->name = NULL;
#endif

    rwlock->writer = NULL;
    rwlock->readers = 0;

    GYROS__LIST_NODE_INIT(&rwlock->rd_task_list);
    GYROS__LIST_NODE_INIT(&rwlock->wr_task_list);
}

void
gyros_rwlock_name(gyros_rwlock_t *rwlock, const char *name)
{
#if GYROS_CONFIG_DEBUG
    rwlock->name = name;
#endif
}

void
gyros_rwlock_rdlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros_error("uninitialized rwlock in rwlock_rdlock", rwlock);
    if (gyros_in_interrupt())
        gyros_error("rwlock_rdlock called from interrupt", rwlock);
    if (rwlock->writer == gyros__state.current)
        gyros_error("rwlock_rdlock deadlock", rwlock);
#endif

    while (unlikely(rwlock->writer ||
                    !gyros__list_empty(&rwlock->wr_task_list)))
    {
        gyros__task_move(gyros__state.current, &rwlock->rd_task_list);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "rwlock_rdlock";
        gyros__state.current->debug_object = rwlock;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
    }
    rwlock->readers++;
    gyros_interrupt_restore(flags);
}

int
gyros_rwlock_try_rdlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags = gyros_interrupt_disable();
    int ret;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros_error("uninitialized rwlock in rwlock_try_rdlock", rwlock);
    if (gyros_in_interrupt())
        gyros_error("rwlock_try_rdlock called from interrupt", rwlock);
#endif

    if (unlikely(rwlock->writer || !gyros__list_empty(&rwlock->wr_task_list)))
        ret = 0;
    else
    {
        rwlock->readers++;
        ret = 1;
    }
    gyros_interrupt_restore(flags);

    return ret;
}

int
gyros_rwlock_rdlock_until(gyros_rwlock_t *rwlock, gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros_error("uninitialized rwlock in rwlock_rdlock", rwlock);
    if (gyros_in_interrupt())
        gyros_error("rwlock_rdlock called from interrupt", rwlock);
    if (rwlock->writer == gyros__state.current)
        gyros_error("gyros_rwlock_rdlock_until deadlock", rwlock);
#endif

    while (unlikely(rwlock->writer ||
                    !gyros__list_empty(&rwlock->wr_task_list)))
    {
        gyros__task_move(gyros__state.current, &rwlock->rd_task_list);
        gyros__task_set_timeout(timeout);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "rwlock_rdlock_until";
        gyros__state.current->debug_object = rwlock;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
        if (gyros__state.current->timed_out)
        {
            gyros_interrupt_restore(flags);
            return 0;
        }
    }

    rwlock->readers++;
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_rwlock_wrlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros_error("uninitialized rwlock in rwlock_wrlock", rwlock);
    if (gyros_in_interrupt())
        gyros_error("rwlock_wrlock called from interrupt", rwlock);
    if (rwlock->writer == gyros__state.current)
        gyros_error("gyros_rwlock_wrlock deadlock", rwlock);
#endif

    while (unlikely((rwlock->writer != 0 &&
                     rwlock->writer != gyros__state.current) ||
                    rwlock->readers))
    {
        gyros__task_move(gyros__state.current, &rwlock->wr_task_list);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "rwlock_wrlock";
        gyros__state.current->debug_object = rwlock;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
    }
    rwlock->writer = gyros__state.current;
    gyros_interrupt_restore(flags);
}

int
gyros_rwlock_try_wrlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags = gyros_interrupt_disable();
    int ret;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros_error("uninitialized rwlock in rwlock_wrlock", rwlock);
    if (gyros_in_interrupt())
        gyros_error("rwlock_wrlock called from interrupt", rwlock);
#endif

    if (unlikely((rwlock->writer != 0 &&
                  rwlock->writer != gyros__state.current) || rwlock->readers))
    {
        ret = 0;
    }
    else
    {
        rwlock->writer = gyros__state.current;
        ret = 1;
    }
    gyros_interrupt_restore(flags);

    return ret;
}

int
gyros_rwlock_wrlock_until(gyros_rwlock_t *rwlock, gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros_error("uninitialized rwlock in rwlock_wrlock", rwlock);
    if (gyros_in_interrupt())
        gyros_error("rwlock_wrlock called from interrupt", rwlock);
    if (rwlock->writer == gyros__state.current)
        gyros_error("gyros_rwlock_wrlock_until deadlock", rwlock);
#endif

    while (unlikely((rwlock->writer != 0 &&
                     rwlock->writer != gyros__state.current) ||
                    rwlock->readers))
    {
        gyros__task_move(gyros__state.current, &rwlock->wr_task_list);
        gyros__task_set_timeout(timeout);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "rwlock_wrlock_until";
        gyros__state.current->debug_object = rwlock;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
        if (gyros__state.current->timed_out)
        {
            gyros_interrupt_restore(flags);
            return 0;
        }
    }

    rwlock->writer = gyros__state.current;
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_rwlock_unlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros_error("uninitialized rwlock in rwlock_unlock", rwlock);
    if (gyros_in_interrupt())
        gyros_error("rwlock_unlock called from interrupt", rwlock);
#endif

    if (rwlock->writer)
    {
#if GYROS_CONFIG_DEBUG
        if (rwlock->writer != gyros__state.current)
            gyros_error("rwlock_unlock called by non owner task", rwlock);
#endif
        rwlock->writer = NULL;
    }
    else
    {
        rwlock->readers--;
#if GYROS_CONFIG_DEBUG
        if (rwlock->readers < 0)
            gyros_error("rwlock_unlock called for unlocked rwlock", rwlock);
#endif
    }
    /* It's important that we don't try to wake any readers if there
     * are queued writers. */
    if (!gyros__list_empty(&rwlock->wr_task_list))
    {
        if (rwlock->readers == 0)
        {
            gyros_task_t *task = TASK(rwlock->wr_task_list.next);

            /* We need to set the writer here to prevent any readers
             * from sneaking in before the writer has had a chance to
             * set it itself when waking up. */
            rwlock->writer = task;
            gyros__task_wake(task);
        }
    }
    else
    {
        /* Move the tasks in reverse order to preserve the order of
         * the tasks (of equal priority) in the list. */
        while (!gyros__list_empty(&rwlock->rd_task_list))
            gyros__task_wake(TASK(rwlock->rd_task_list.prev));
    }
    gyros_interrupt_restore(flags);
    gyros__cond_reschedule();
}
