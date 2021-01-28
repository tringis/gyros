/**************************************************************************
 * Copyright (c) 2002-2021, Tobias Ringström <tobias@ringis.se>
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
#include <gyros/smq.h>
#include <gyros/private/trace.h>

#include <stddef.h>
#include <string.h>

#include <gyros/interrupt.h>

#include "private.h"

void
gyros_smq_init(gyros_smq_t *smq,
               void *memory,
               unsigned msg_size,
               unsigned msg_count)
{
    GYROS_DEBUG_INFO_INIT(smq, GYROS_SMQ_DEBUG_MAGIC);
    GYROS__LIST_NODE_INIT(&smq->task_list);
    smq->memory = memory;
    smq->msg_size = msg_size;
    smq->total = msg_count;
    smq->used = 0;
    smq->read_pos = 0;
    smq->write_pos = 0;
}

bool
gyros_smq_send(gyros_smq_t *smq, const void *msg, bool block)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (smq->debug_info.magic != GYROS_SMQ_DEBUG_MAGIC)
        gyros__error("uninitialized smq in smq_send", smq);
#endif

    GYROS__TRACE_SMQ(SEND, smq);
    while (GYROS_UNLIKELY(smq->used >= smq->total))
    {
        if (!block || gyros_in_interrupt())
        {
            gyros_interrupt_restore(flags);
            return false;
        }
        GYROS__TRACE_SMQ(RECEIVE_BLOCKED, smq);
        gyros__task_move(gyros.current, &smq->task_list);
        GYROS_DEBUG_SET_STATE2(gyros.current, "smq_receive", smq);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
    }

    memcpy(smq->memory + smq->write_pos * smq->msg_size, msg, smq->msg_size);
    if (++smq->write_pos == smq->total)
        smq->write_pos = 0;
    smq->used++;

    if (gyros__list_empty(&smq->task_list))
        gyros_interrupt_restore(flags);
    else
    {
        gyros__task_wake(TASK(smq->task_list.next));
        gyros__cond_reschedule();
        gyros_interrupt_restore(flags);
    }

    return true;
}

bool
gyros_smq_receive(gyros_smq_t *smq, void *msg)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (smq->debug_info.magic != GYROS_SMQ_DEBUG_MAGIC)
        gyros__error("uninitialized smq in smq_receive", smq);
    if (!gyros_in_interrupt() && gyros_interrupts_disabled())
        gyros__error("smq_receive called with interrupts disabled", smq);
#endif

    flags = gyros_interrupt_disable();
    while (GYROS_UNLIKELY(smq->used == 0))
    {
        if (gyros_in_interrupt())
        {
            gyros_interrupt_restore(flags);
            return false;
        }
        GYROS__TRACE_SMQ(RECEIVE_BLOCKED, smq);
        gyros__task_move(gyros.current, &smq->task_list);
        GYROS_DEBUG_SET_STATE2(gyros.current, "smq_receive", smq);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
    }

    memcpy(msg, smq->memory + smq->read_pos * smq->msg_size, smq->msg_size);
    if (++smq->read_pos == smq->total)
        smq->read_pos = 0;
    smq->used--;

    GYROS__TRACE_SMQ(RECEIVED, smq);
    gyros_interrupt_restore(flags);

    return true;
}

bool
gyros_smq_receive_until(gyros_smq_t *smq,
                        void *msg,
                        gyros_abstime_t timeout)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    if (smq->debug_info.magic != GYROS_SMQ_DEBUG_MAGIC)
        gyros__error("uninitialized smq in smq_receive_until", smq);
    if (gyros_in_interrupt())
        gyros__error("smq_receive_until called from interrupt", smq);
    if (gyros_interrupts_disabled())
        gyros__error("smq_receive_until called with interrupts disabled", smq);
#endif

    flags = gyros_interrupt_disable();
    while (GYROS_UNLIKELY(smq->used == 0))
    {
        if (!gyros__task_set_timeout(timeout))
        {
            gyros_interrupt_restore(flags);
            return false;
        }
        GYROS__TRACE_SMQ(RECEIVE_BLOCKED, smq);
        gyros__task_move(gyros.current, &smq->task_list);
        GYROS_DEBUG_SET_STATE2(gyros.current, "smq_receive_until", smq);
        gyros__reschedule();
        gyros_interrupt_restore(flags);
        flags = gyros_interrupt_disable();
        if (smq->used == 0)
        {
            gyros_interrupt_restore(flags);
            return false;
        }
    }

    memcpy(msg, smq->memory + smq->read_pos * smq->msg_size, smq->msg_size);
    if (++smq->read_pos == smq->total)
        smq->read_pos = 0;
    smq->used--;

    GYROS__TRACE_SMQ(RECEIVED, smq);
    gyros_interrupt_restore(flags);

    return true;
}

bool
gyros_smq_empty(gyros_smq_t *smq)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (smq->debug_info.magic != GYROS_SMQ_DEBUG_MAGIC)
        gyros__error("uninitialized smq in smq_send", smq);
#endif

    bool empty = smq->used == 0;
    gyros_interrupt_restore(flags);
    return empty;
}
