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

#include "private.h"

static void (*s_context_hook)(gyros_task_t *current, gyros_task_t *next);
static void (*s_irq_hook)(void);

void
gyros__context_hook(void)
{
    if (s_context_hook)
        s_context_hook(gyros__state.current, TASK(gyros__state.running.next));
}

void
gyros__irq_hook(void)
{
    if (s_irq_hook)
        s_irq_hook();
}

void
gyros_set_context_hook(void (*context_hook)(gyros_task_t *current,
                                            gyros_task_t *next))
{
    unsigned long flags = gyros_interrupt_disable();

    s_context_hook = context_hook;
    gyros_interrupt_restore(flags);
}

void gyros_set_irq_hook(void (*irq_hook)(void))
{
    unsigned long flags = gyros_interrupt_disable();

    s_irq_hook = irq_hook;
    gyros_interrupt_restore(flags);
}
