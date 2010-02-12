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
#include <gyros/private/target.h>

void
gyros__target_task_init(gyros_task_t *task,
                        void (*entry)(void *arg),
                        void *arg,
                        void *stack,
                        int stack_size)
{
    unsigned long *stack_top = (unsigned long *)((unsigned long)stack +
                                                 stack_size);
    unsigned long *sp = stack_top;

    *--sp = 0;                               /* xPSR */
    *--sp = (unsigned long)entry;            /* PC */
    *--sp = (unsigned long)gyros__task_exit; /* LR */
    *--sp = 0;                               /* R12 */
    *--sp = 0;                               /* R3 */
    *--sp = 0;                               /* R2 */
    *--sp = 0;                               /* R1 */
    *--sp = (unsigned long)arg;              /* R0 */

#if GYROS_CONFIG_STACK_USED
    {
        unsigned long *p = task->stack;

        do
            *p++ = 0xeeeeeeee;
        while (p != stack_top);
    }
#endif
}
