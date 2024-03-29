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
#include <gyros/compiler.h>
#include <gyros/interrupt.h>
#include <gyros/private/trace.h>
#include <gyros/rwlock.h>

#include <stddef.h>

#include "private.h"

void
gyros_rwlock_init(gyros_rwlock_t *rwlock)
{
    GYROS_DEBUG_INFO_INIT(rwlock, GYROS_RWLOCK_DEBUG_MAGIC);

    rwlock->writer = NULL;
    rwlock->readers = 0;

    GYROS__LIST_NODE_INIT(&rwlock->rd_task_list);
    GYROS__LIST_NODE_INIT(&rwlock->wr_task_list);
}

void
gyros_rwlock_rdlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_info.magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized rwlock in rwlock_rdlock", rwlock);
    if (gyros_in_interrupt())
        gyros__error("rwlock_rdlock called from interrupt", rwlock);
    if (gyros_interrupts_disabled())
        gyros__error("rwlock_rdlock called with interrupts disabled", rwlock);
    if (rwlock->writer == _gyros.current)
        gyros__error("rwlock_rdlock deadlock", rwlock);
#endif

    flags = gyros_interrupt_disable();

    while (GYROS_UNLIKELY(rwlock->writer ||
                          !gyros__list_empty(&rwlock->wr_task_list)))
    {
        GYROS__TRACE_RWLOCK(RD_BLOCKED, rwlock);
        gyros__task_move(_gyros.current, &rwlock->rd_task_list);
        GYROS_DEBUG_SET_STATE2(_gyros.current, "rwlock_rdlock", rwlock);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
    }
    rwlock->readers++;
    GYROS__TRACE_RWLOCK(RD_AQUIRED, rwlock);
    gyros_interrupt_restore(flags);
}

bool
gyros_rwlock_try_rdlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags;
    bool ret;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_info.magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized rwlock in rwlock_try_rdlock", rwlock);
    if (gyros_in_interrupt())
        gyros__error("rwlock_try_rdlock called from interrupt", rwlock);
    if (gyros_interrupts_disabled())
    {
        gyros__error("rwlock_try_rdlock called with interrupts disabled",
                     rwlock);
    }
#endif

    flags = gyros_interrupt_disable();

    if (GYROS_UNLIKELY(rwlock->writer ||
                       !gyros__list_empty(&rwlock->wr_task_list)))
    {
        ret = false;
    }
    else
    {
        GYROS__TRACE_RWLOCK(RD_AQUIRED, rwlock);
        rwlock->readers++;
        ret = true;
    }
    gyros_interrupt_restore(flags);

    return ret;
}

bool
gyros_rwlock_rdlock_until(gyros_rwlock_t *rwlock, gyros_abstime_t timeout)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_info.magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized rwlock in rwlock_rdlock", rwlock);
    if (gyros_in_interrupt())
        gyros__error("rwlock_rdlock_until called from interrupt", rwlock);
    if (gyros_interrupts_disabled())
    {
        gyros__error("rwlock_rdlock_until called with interrupts disabled",
                     rwlock);
    }
    if (rwlock->writer == _gyros.current)
        gyros__error("rwlock_rdlock_until deadlock", rwlock);
#endif

    flags = gyros_interrupt_disable();

    while (GYROS_UNLIKELY(rwlock->writer ||
                          !gyros__list_empty(&rwlock->wr_task_list)))
    {
        if (!gyros__task_set_timeout(timeout))
        {
            gyros_interrupt_restore(flags);
            return false;
        }
        GYROS__TRACE_RWLOCK(RD_BLOCKED, rwlock);
        gyros__task_move(_gyros.current, &rwlock->rd_task_list);
        GYROS_DEBUG_SET_STATE2(_gyros.current, "rwlock_rdlock_until", rwlock);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
        if (_gyros.current->timed_out)
        {
            gyros_interrupt_restore(flags);
            return false;
        }
    }

    rwlock->readers++;
    GYROS__TRACE_RWLOCK(RD_AQUIRED, rwlock);
    gyros_interrupt_restore(flags);

    return true;
}

void
gyros_rwlock_wrlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_info.magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized rwlock in rwlock_wrlock", rwlock);
    if (gyros_in_interrupt())
        gyros__error("rwlock_wrlock called from interrupt", rwlock);
    if (gyros_interrupts_disabled())
        gyros__error("rwlock_wrlock called with interrupts disabled", rwlock);
    if (rwlock->writer == _gyros.current)
        gyros__error("rwlock_wrlock deadlock", rwlock);
#endif

    flags = gyros_interrupt_disable();

    while (GYROS_UNLIKELY((rwlock->writer != 0 &&
                           rwlock->writer != _gyros.current) ||
                          rwlock->readers))
    {
        GYROS__TRACE_RWLOCK(WR_BLOCKED, rwlock);
        gyros__task_move(_gyros.current, &rwlock->wr_task_list);
        GYROS_DEBUG_SET_STATE2(_gyros.current, "rwlock_wrlock", rwlock);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
    }
    rwlock->writer = _gyros.current;
    GYROS__TRACE_RWLOCK(WR_AQUIRED, rwlock);
    gyros_interrupt_restore(flags);
}

bool
gyros_rwlock_try_wrlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags;
    bool ret;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_info.magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized rwlock in rwlock_wrlock", rwlock);
    if (gyros_in_interrupt())
        gyros__error("rwlock_try_wrlock called from interrupt", rwlock);
    if (gyros_interrupts_disabled())
    {
        gyros__error("rwlock_try_wrlock called with interrupts disabled",
                     rwlock);
    }
#endif

    flags = gyros_interrupt_disable();

    if (GYROS_UNLIKELY((rwlock->writer != 0 &&
                        rwlock->writer != _gyros.current) || rwlock->readers))
    {
        ret = false;
    }
    else
    {
        rwlock->writer = _gyros.current;
        GYROS__TRACE_RWLOCK(WR_AQUIRED, rwlock);
        ret = true;
    }
    gyros_interrupt_restore(flags);

    return ret;
}

bool
gyros_rwlock_wrlock_until(gyros_rwlock_t *rwlock, gyros_abstime_t timeout)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_info.magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized rwlock in rwlock_wrlock", rwlock);
    if (gyros_in_interrupt())
        gyros__error("rwlock_wrlock_until called from interrupt", rwlock);
    if (gyros_interrupts_disabled())
    {
        gyros__error("rwlock_wrlock_until called with interrupts disabled",
                     rwlock);
    }
    if (rwlock->writer == _gyros.current)
        gyros__error("rwlock_wrlock_until deadlock", rwlock);
#endif

    flags = gyros_interrupt_disable();

    while (GYROS_UNLIKELY((rwlock->writer != 0 &&
                           rwlock->writer != _gyros.current) ||
                          rwlock->readers))
    {
        if (!gyros__task_set_timeout(timeout))
        {
            gyros_interrupt_restore(flags);
            return false;
        }
        GYROS__TRACE_RWLOCK(WR_BLOCKED, rwlock);
        gyros__task_move(_gyros.current, &rwlock->wr_task_list);
        GYROS_DEBUG_SET_STATE2(_gyros.current, "rwlock_wrlock_until", rwlock);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
        if (_gyros.current->timed_out)
        {
            gyros_interrupt_restore(flags);
            return false;
        }
    }

    rwlock->writer = _gyros.current;
    GYROS__TRACE_RWLOCK(WR_AQUIRED, rwlock);
    gyros_interrupt_restore(flags);

    return true;
}

void
gyros_rwlock_unlock(gyros_rwlock_t *rwlock)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (rwlock->debug_info.magic != GYROS_RWLOCK_DEBUG_MAGIC)
        gyros__error("uninitialized rwlock in rwlock_unlock", rwlock);
    if (gyros_in_interrupt())
        gyros__error("rwlock_unlock called from interrupt", rwlock);
    if (gyros_interrupts_disabled())
        gyros__error("rwlock_unlock called with interrupts disabled", rwlock);
#endif

    flags = gyros_interrupt_disable();

    if (rwlock->writer)
    {
#if GYROS_CONFIG_DEBUG
        if (rwlock->writer != _gyros.current)
            gyros__error("rwlock_unlock called by non owner task", rwlock);
#endif
        GYROS__TRACE_RWLOCK(WR_UNLOCK, rwlock);
        rwlock->writer = NULL;
    }
    else
    {
        GYROS__TRACE_RWLOCK(RD_UNLOCK, rwlock);
        rwlock->readers--;
#if GYROS_CONFIG_DEBUG
        if (rwlock->readers < 0)
            gyros__error("rwlock_unlock called for unlocked rwlock", rwlock);
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
    gyros__cond_reschedule();
    gyros_interrupt_restore(flags);
}
