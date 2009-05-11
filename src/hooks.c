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
#include <gyros/hooks.h>
#include <gyros/interrupt.h>
#include <gyros/private/trace.h>

#include <stddef.h>

#include "private.h"

#if GYROS_CONFIG_CONTEXT_HOOK
static void (*s_context_hook)(gyros_task_t *current, gyros_task_t *next);
#endif

#if GYROS_CONFIG_IRQ_HOOK
static void (*s_irq_hook)(void);
#endif

#if GYROS_CONFIG_CONTEXT_HOOK || GYROS_CONFIG_TRACE
void
gyros__context_hook(void)
{
    gyros_task_t *next = TASK(gyros__state.running.next);

#if GYROS_CONFIG_TRACE
    if (unlikely(gyros__trace_enabled))
    {
        gyros_trace_t *t = gyros__trace(GYROS_TRACE_CONTEXT);

        t->info.context.prev = gyros__state.current;
        t->info.context.next = next;
    }
#endif

#if GYROS_CONFIG_CONTEXT_HOOK
    if (unlikely(s_context_hook != NULL))
        s_context_hook(gyros__state.current, next);
#endif
}
#endif

#if GYROS_CONFIG_IRQ_HOOK || GYROS_CONFIG_TRACE
void
gyros__irq_hook(void)
{
#if GYROS_CONFIG_TRACE
    if (unlikely(gyros__trace_enabled))
        gyros__trace(GYROS_TRACE_IRQ)->task = NULL;
#endif

#if GYROS_CONFIG_IRQ_HOOK
    if (unlikely(s_irq_hook != NULL))
        s_irq_hook();
#endif
}
#endif

#if GYROS_CONFIG_CONTEXT_HOOK
void
gyros_set_context_hook(void (*context_hook)(gyros_task_t *current,
                                            gyros_task_t *next))
{
    unsigned long flags = gyros_interrupt_disable();

    s_context_hook = context_hook;
    gyros_interrupt_restore(flags);
}
#endif

#if GYROS_CONFIG_IRQ_HOOK
void gyros_set_irq_hook(void (*irq_hook)(void))
{
    unsigned long flags = gyros_interrupt_disable();

    s_irq_hook = irq_hook;
    gyros_interrupt_restore(flags);
}
#endif
