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
#include <string.h>

#include <gyros/arm/arm_defs.h>

#include "../private.h"

void
gyros__target_task_init(gyros_task_t *task,
                        void (*entry)(void *arg),
                        void *arg,
                        void *stack,
                        int stack_size)
{
#if GYROS_CONFIG_STACK_USED
    uint32_t *p = task->stack;
    uint32_t *e = (uint32_t*)((uint32_t)task->stack + task->stack_size);
#endif

    task->context.r[0] = (uint32_t)arg;
    task->context.sp = (uint32_t)stack + stack_size;
    task->context.lr = (uint32_t)gyros__task_exit;
    task->context.pc = (uint32_t)entry + 4;
    task->context.psr = ARM_MODE_SYS;

#if GYROS_CONFIG_STACK_USED
    do
        *p++ = 0xeeeeeeee;
    while (p != e);
#endif
}

#if GYROS_CONFIG_STACK_USED
int
gyros_task_stack_used(gyros_task_t *task)
{
    uint32_t *p = task->stack;
    uint32_t *e = (uint32_t*)((uint32_t)task->stack + task->stack_size);

    while (p != e && *p == 0xeeeeeeee)
        p++;

    return (uint32_t)e - (uint32_t)p;
}
#endif
