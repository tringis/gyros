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
#include <string.h>

#include <gyros/interrupt.h>
#include <gyros/private/target.h>
#include <gyros/config.h>
#include <gyros/time.h>

#include <gyros/arch/armv7-m/target.h>

#include "nvic.h"

#define STACK_WORDS (GYROS_CONFIG_EXCEPTION_STACK_SIZE / sizeof(unsigned long))

void gyros__arch_enable_thread_stack(unsigned long *stack_top);

static unsigned long s_exception_stack[STACK_WORDS];

#if !GYROS_CONFIG_DYNTICK
static gyros_abstime_t s_time;

void
SysTick_Handler(void)
{
    gyros__tick(++s_time);
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    gyros_abstime_t time = s_time;

    gyros_interrupt_restore(flags);

    return time;
}
#endif

void
gyros__arch_init(void)
{
#if GYROS_CONFIG_STACK_USED
    memset(s_exception_stack, 0xee, sizeof(s_exception_stack));
#endif

    gyros__arch_enable_thread_stack(s_exception_stack + STACK_WORDS);

#if !GYROS_CONFIG_DYNTICK
    NVIC_SYSTICK_RELOAD = GYROS_CONFIG_CORE_HZ / GYROS_CONFIG_HZ - 1;
    NVIC_SYSTICK_CURRENT = 0; /* reset-on-write register */
    NVIC_SYSTICK_CTLST = (NVIC_SYSTICK_CTLST & ~0x00010007) | 7;
    NVIC_SYSTICK_PRIO = GYROS_CONFIG_SYSTICK_PRIORITY;
#endif

    NVIC_PENDSV_PRIO = 0xff;
}

void
gyros__target_task_init(gyros_task_t *task,
                        void (*entry)(void *arg),
                        void *arg,
                        void *stack,
                        int stack_size)
{
    unsigned long *sp = (unsigned long *)stack +
                        stack_size / sizeof(unsigned long);

#if GYROS_CONFIG_STACK_USED
    memset(stack, 0xee, stack_size);
#endif

    /* ARM EABI requires the stack to be 8-byte aligned */
    if ((unsigned long)sp & 0x7)
        --sp;

    *--sp = 1U << 24;                        /* xPSR (thumb mode) */
    *--sp = (unsigned long)entry;            /* PC */
    *--sp = (unsigned long)gyros__task_exit; /* LR */
    *--sp = 0;                               /* R12 */
    *--sp = 0;                               /* R3 */
    *--sp = 0;                               /* R2 */
    *--sp = 0;                               /* R1 */
    *--sp = (unsigned long)arg;              /* R0 */

    task->context.sp = (unsigned long)sp;
}

void
gyros__idle(void)
{
#if GYROS_CONFIG_IDLE_WFI
    __asm__ __volatile__("wfi" ::: "memory");
#endif
}

#if GYROS_CONFIG_STACK_USED
int
gyros_isr_stack_used(void)
{
    unsigned long *p = s_exception_stack;
    unsigned long *e = s_exception_stack + STACK_WORDS;

    while (p != e && *p == 0xeeeeeeee)
        p++;

    return (unsigned long)e - (unsigned long)p;
}

int
gyros_isr_stack_size(void)
{
    return 4 * STACK_WORDS;
}
#endif
