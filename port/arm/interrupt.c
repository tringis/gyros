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

#ifdef __thumb__
#error This file must be compiled in ARM mode
#endif

#if GYROS_CONFIG_THUMB
unsigned long
gyros_interrupt_disable(void)
{
    unsigned long temp, flags;

    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "mrs    %1, cpsr\n\t"
        "orr    %0, %1, #0xc0\n\t"
        "msr    cpsr_c, %0\n\t"
        : "=l" (temp), "=&l" (flags) :: "memory");

    return flags;
}

void
gyros_interrupt_restore(unsigned long flags)
{
    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "msr    cpsr_c, %0\n\t"
        :: "r" (flags) : "memory");
}

int
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

void
gyros__interrupt_enable(void)
{
    unsigned long temp;

    /* Inline assembly to clear the IRQ and FIQ bits in CPSR. */
    __asm__ __volatile__(
        "mrs    %0, cpsr\n\t"
        "bic    %0, %0, #0xc0\n\t"
        "msr    cpsr_c, %0"
        : "=l" (temp) :: "memory");
}
