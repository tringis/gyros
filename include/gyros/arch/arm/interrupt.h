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

#include <gyros/target/config.h>
#include <gyros/arch/arm/arm_defs.h>

#if !GYROS_CONFIG_THUMB
static inline unsigned long
gyros_interrupt_disable(void)
{
    unsigned long temp, flags;

    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "mrs    %1, cpsr\n\t"
        "orr    %0, %1, #0xc0\n\t"
        "msr    cpsr_c, %0\n\t"
        : "=r" (temp), "=&r" (flags) :: "memory");

    return flags;
}

/* Restore interrupts (IRQ and FIQ) in the ARM core. */
static inline void
gyros_interrupt_restore(unsigned long flags)
{
    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "msr    cpsr_c, %0\n\t"
        :: "r" (flags) : "memory");
}

static inline int
gyros_in_interrupt(void)
{
    unsigned long cpsr;

    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "mrs    %0, cpsr\n\t"
        : "=r" (cpsr) :: "memory");

    return (cpsr & 0x1f) != ARM_MODE_SYS;
}
#endif

/* Reschedule, i.e. make sure the right task is running. */
static inline void
gyros__reschedule(void)
{
    /* The interrupt exit code always reschedules for ARM */
    if (!gyros_in_interrupt())
        __asm__ __volatile__("svc    #0" ::: "memory");
}

/* Enable interrupts in the ARM core. */
void gyros__interrupt_enable(void);

#endif
