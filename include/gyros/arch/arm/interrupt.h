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
#ifndef INCLUDE__gyros_arm_interrupt_h__200212292232
#define INCLUDE__gyros_arm_interrupt_h__200212292232

#include <gyros-config.h>
#include <gyros/compiler.h>
#include <gyros/arch/arm/arm_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

GYROS_ALWAYS_INLINE unsigned long
gyros_interrupt_disable(void)
{
#ifdef __thumb__
    unsigned long gyros__arm_interrupt_disable(void);
    return gyros__arm_interrupt_disable();
#else
    unsigned long temp, flags;

    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "mrs    %1, cpsr\n\t"
        "orr    %0, %1, #0xc0\n\t"
        "msr    cpsr_c, %0\n\t"
        : "=r" (temp), "=&r" (flags) :: "memory");

    return flags;
#endif
}

/* Restore interrupts (IRQ and FIQ) in the ARM core. */
GYROS_ALWAYS_INLINE void
gyros_interrupt_restore(unsigned long flags)
{
#ifdef __thumb__
    void gyros__arm_interrupt_restore(unsigned long flags);
    return gyros__arm_interrupt_restore(flags);
#else
    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "msr    cpsr_c, %0\n\t"
        :: "r" (flags) : "memory");
#endif
}

GYROS_ALWAYS_INLINE int
gyros__get_cpsr(void)
{
#ifdef __thumb__
    int gyros__arm_get_cpsr(void);
    return gyros__arm_get_cpsr();
#else
    unsigned long cpsr;

    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "mrs    %0, cpsr\n\t"
        : "=r" (cpsr) :: "memory");

    return cpsr;
#endif
}

GYROS_ALWAYS_INLINE int
gyros_in_interrupt(void)
{
    return (gyros__get_cpsr() & 0x1f) != ARM_MODE_SYS;
}

GYROS_ALWAYS_INLINE int
gyros_interrupts_disabled(void)
{
    return (gyros__get_cpsr() & ARM_IRQ_BIT) != 0;
}

/* Reschedule, i.e. make sure the right task is running. */
GYROS_ALWAYS_INLINE void
gyros__reschedule(void)
{
    /* The interrupt exit code always reschedules for ARM */
    if (!gyros_in_interrupt())
        __asm__ __volatile__("svc    #0" ::: "memory");
}

/* Reschedule, i.e. make sure the right task is running. */
GYROS_ALWAYS_INLINE void
gyros__tick_reschedule(void)
{
    /* The interrupt exit code always reschedules for ARM */
}

/* Enable interrupts in the ARM core. */
void gyros__interrupt_enable(void);

#ifdef __cplusplus
}
#endif

#endif
