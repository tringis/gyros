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
#include <gyros/mq.h>

#include <stddef.h>

#include <gyros/interrupt.h>

#include "private.h"

void
gyros_mq_init(gyros_mq_t *mq)
{
#if GYROS_CONFIG_DEBUG
    mq->debug_magic = GYROS_MQ_DEBUG_MAGIC;
#endif

    GYROS__LIST_NODE_INIT(&mq->msg_list);
    GYROS__LIST_NODE_INIT(&mq->task_list);
}

void
gyros_mq_send(gyros_mq_t *mq, void *msg)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_mq_msghdr_t *msghdr = msg;

#if GYROS_CONFIG_DEBUG
    if (mq->debug_magic != GYROS_MQ_DEBUG_MAGIC)
        gyros_error("uninitialized mq in mq_send");
#endif

    gyros__list_insert_before(&msghdr->list, &mq->msg_list);
    if (gyros__list_empty(&mq->task_list))
        gyros_interrupt_restore(flags);
    else
    {
        gyros__task_wake(TASK(mq->task_list.next));
        gyros_interrupt_restore(flags);
        if (!gyros_in_interrupt())
            gyros__cond_reschedule();
    }
}

void*
gyros_mq_receive(gyros_mq_t *mq)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_mq_msghdr_t *msghdr;

#if GYROS_CONFIG_DEBUG
    if (mq->debug_magic != GYROS_MQ_DEBUG_MAGIC)
        gyros_error("uninitialized mq in mq_send");
#endif

    while (unlikely(gyros__list_empty(&mq->msg_list)))
    {
        if (gyros_in_interrupt())
        {
            gyros_interrupt_restore(flags);
            return NULL;
        }
        gyros__task_move(gyros__state.current, &mq->task_list);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "mq_receive";
        gyros__state.current->debug_object = mq;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
    }
    msghdr = GYROS__LIST_CONTAINER(mq->msg_list.next, gyros_mq_msghdr_t, list);
    gyros__list_remove(mq->msg_list.next);
    gyros_interrupt_restore(flags);

    return msghdr;
}

void*
gyros_mq_receive_until(gyros_mq_t *mq, gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_mq_msghdr_t *msghdr;

#if GYROS_CONFIG_DEBUG
    if (mq->debug_magic != GYROS_MQ_DEBUG_MAGIC)
        gyros_error("uninitialized mq in mq_send");
    if (gyros_in_interrupt())
        gyros_error("mq_receive_until called from interrupt");
#endif

    if (unlikely(gyros__list_empty(&mq->msg_list)))
    {
        gyros__task_move(gyros__state.current, &mq->task_list);
        gyros__task_set_timeout(timeout);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "mq_receive_until";
        gyros__state.current->debug_object = mq;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
        if (gyros__list_empty(&mq->msg_list))
        {
            gyros_interrupt_restore(flags);
            return NULL;
        }
    }
    msghdr = GYROS__LIST_CONTAINER(mq->msg_list.next, gyros_mq_msghdr_t, list);
    gyros__list_remove(mq->msg_list.next);
    gyros_interrupt_restore(flags);

    return msghdr;
}
