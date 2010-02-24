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
#include <gyros/arch/arm/arm_defs.h>
#include <gyros/private/target.h>

void
gyros__target_task_init(gyros_task_t *task,
                        void (*entry)(void *arg),
                        void *arg,
                        void *stack,
                        int stack_size)
{
    task->context.r[0] = (unsigned long)arg;
    task->context.sp = (unsigned long)stack + stack_size;
    task->context.lr = (unsigned long)gyros__task_exit;
    task->context.pc = (unsigned long)entry + 4;
    task->context.psr = ARM_MODE_SYS;
    if ((unsigned long)entry % 1)
        task->context.psr |= ARM_THUMB_BIT;

#if GYROS_CONFIG_STACK_USED
    {
        unsigned long *p = task->stack;
        unsigned long *e = (unsigned long*)((unsigned long)task->stack +
                                            task->stack_size);

        do
            *p++ = 0xeeeeeeee;
        while (p != e);
    }
#endif
}
