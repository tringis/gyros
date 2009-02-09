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
#include <gyros/message.h>

#include <gyros/interrupt.h>
#include <gyros/target/config.h>

#include <stddef.h>

#include "private.h"

#if GYROS_CONFIG_MESSAGE_QUEUE
void
gyros_send(gyros_task_t *task, void *msg)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_msghdr_t *msghdr = msg;

#if GYROS_CONFIG_DEBUG
    if (task->debug_magic != GYROS_TASK_DEBUG_MAGIC)
        gyros_error("send to non-task");
#endif

    msghdr->sender = gyros_in_interrupt() ? NULL : gyros__state.current;
    gyros__list_insert_before(&msghdr->list, &task->msg_list);
    if (task->receiving)
    {
        task->receiving = 0;
        gyros__task_wake(task);
        gyros_interrupt_restore(flags);
        if (!gyros_in_interrupt())
            gyros__cond_reschedule();
    }
    else
        gyros_interrupt_restore(flags);
}

void*
gyros_receive(void)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_msghdr_t *msghdr;

#if GYROS_CONFIG_DEBUG
    if (gyros_in_interrupt())
        gyros_error("receive called from interrupt");
#endif

    while (gyros__list_empty(&gyros__state.current->msg_list))
    {
        gyros__state.current->receiving = 1;
        gyros__list_remove(&gyros__state.current->main_list);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "receive";
        gyros__state.current->debug_object = NULL;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
        gyros__state.current->receiving = 0;
    }
    msghdr = GYROS__LIST_CONTAINER(gyros__state.current->msg_list.next,
                                   gyros_msghdr_t, list);
    gyros__list_remove(gyros__state.current->msg_list.next);
    gyros_interrupt_restore(flags);

    return msghdr;
}

void*
gyros_receive_until(gyros_abstime_t timeout)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_msghdr_t *msghdr;

#if GYROS_CONFIG_DEBUG
    if (gyros_in_interrupt())
        gyros_error("receive called from interrupt");
#endif

    if (gyros__list_empty(&gyros__state.current->msg_list))
    {
        gyros__state.current->receiving = 1;
        gyros__list_remove(&gyros__state.current->main_list);
        gyros__task_set_timeout(timeout);
#if GYROS_CONFIG_DEBUG
        gyros__state.current->debug_state = "receive_until";
        gyros__state.current->debug_object = NULL;
#endif
        gyros_interrupt_restore(flags);
        gyros__cond_reschedule();
        flags = gyros_interrupt_disable();
        gyros__state.current->receiving = 0;
        if (gyros__list_empty(&gyros__state.current->msg_list))
        {
            gyros_interrupt_restore(flags);
            return NULL;
        }
    }
    msghdr = GYROS__LIST_CONTAINER(gyros__state.current->msg_list.next,
                                   gyros_msghdr_t, list);
    gyros__list_remove(gyros__state.current->msg_list.next);
    gyros_interrupt_restore(flags);

    return msghdr;
}

#endif
