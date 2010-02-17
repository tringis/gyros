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
#include <gyros/interrupt.h>
#include <gyros/private/target.h>
#include <gyros/target/config.h>
#include <gyros/time.h>

#include <gyros/arch/armv7-m/target.h>

#include "nvic.h"

static void pendsv_handler(void) __attribute__((__naked__));

static unsigned long s_exception_stack[GYROS_CONFIG_EXCEPTION_STACK_SIZE /
                                       sizeof(unsigned long)];

#if !GYROS_CONFIG_DYNTICK
static gyros_abstime_t s_time;

static void
systick_handler(void)
{
    NVIC_SYSTICK_CTLST;
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

static void
pendsv_handler(void)
{
    __asm__ __volatile__(
#if GYROS_CONFIG_CONTEXT_HOOK || GYROS_CONFIG_TRACE
        "push    {lr}\n"
        "ldr	 r0, =gyros__context_hook\n"
        "blx     r0\n"
        "pop     {lr}\n"
#endif

        "mrs     r3, psp\n"
        "stmdb   r3, {r4-r11}\n"         /* Write registers to task stack. */
        "ldr     r0, =gyros\n"           /* r0 = &gyros.current */
        "ldr     r1, [r0]\n"             /* r1 = gyros.current */
        "str     r3, [r1]\n"             /* gyros.current->context.sp = PSP */

        "ldr     r2, [r0, #4]\n"         /* r2 = gyros.running.next */
        "subs    r2, r2, #4\n"           /* r2 = TASK(gyros.running.next) */
        "str     r2, [r0]\n"             /* gyros.current = r2 */

        "ldr     r3, [r2]\n"             /* r3 = SP of new task */
        "ldmdb   r3, {r4-r11}\n"         /* Load regs for new task */
        "msr     psp, r3\n"              /* Set PSP to SP of new task */
        "bx      lr\n"
        ::: "memory");
}

void
gyros__arch_init(void)
{
    unsigned long *vtab = (unsigned long*)NVIC_VTABOFFSET;
    unsigned long temp;
#if GYROS_CONFIG_STACK_USED
    unsigned long *p = s_exception_stack;
    int i;

    for (i = 0;
         i < GYROS_CONFIG_EXCEPTION_STACK_SIZE / sizeof(unsigned long);
         ++i)
    {
        *p++ = 0xeeeeeeee;
    }
#endif

    vtab[14] = (unsigned long)pendsv_handler;

#if !GYROS_CONFIG_DYNTICK
    vtab[15] = (unsigned long)systick_handler;

    NVIC_SYSTICK_RELOAD = GYROS_CONFIG_CORE_HZ /
                          GYROS_CONFIG_TIMER_HZ - 1;
    NVIC_SYSTICK_CURRENT = 0; /* reset on write register */
    NVIC_SYSTICK_CTLST = (NVIC_SYSTICK_CTLST & ~0x00010007) | 7;
#endif

    __asm__ __volatile__(
        "mov     %0, sp\n"
        "msr     psp, %0\n"              /* PSP = SP */
        "msr     msp, %1\n"              /* MSP = exception_stack top */
        "mov     %0, #2\n"
        "msr     control, %0\n"          /* CONTROL = 2 */
        : "=&r" (temp)
        : "r" ((unsigned long)s_exception_stack +
               GYROS_CONFIG_EXCEPTION_STACK_SIZE)
        : "memory");
}

void
gyros__target_task_init(gyros_task_t *task,
                        void (*entry)(void *arg),
                        void *arg,
                        void *stack,
                        int stack_size)
{
    unsigned long *sp = (unsigned long *)((unsigned long)stack +
                                          stack_size);

#if GYROS_CONFIG_STACK_USED
    {
        unsigned long *stack_top = sp;
        unsigned long *p = task->stack;

        do
            *p++ = 0xeeeeeeee;
        while (p != stack_top);
    }
#endif

    *--sp = 1U << 24;                        /* xPSR (thumb mode) */
    *--sp = (unsigned long)entry;            /* PC */
    *--sp = (unsigned long)gyros__task_exit; /* LR */
    *--sp = 0;                               /* R12 */
    *--sp = 0;                               /* R3 */
    *--sp = 0;                               /* R2 */
    *--sp = 0;                               /* R1 */
    *--sp = (unsigned long)arg;              /* R0 */

    task->context.sp = (unsigned long)sp;

#if GYROS_CONFIG_STACK_USED
    {
        int i;
        for (i = 0; i < 9; ++i)
            *--sp = 0;
    }
#endif
}

void
gyros__idle(void)
{
    __asm__ __volatile__("wfi" ::: "memory"); 
}
