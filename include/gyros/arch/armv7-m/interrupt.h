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
#ifndef INCLUDE__gyros_armv7_m_interrupt_h__201002062157
#define INCLUDE__gyros_armv7_m_interrupt_h__201002062157

#include <gyros/config.h>

static inline unsigned long
gyros_interrupt_disable(void)
{
    unsigned long flags;

    __asm__ __volatile__(
        "mrs    %0, basepri\n\t"
        "msr    basepri_max, %1\n\t"
        : "=&r" (flags) : "r" (GYROS_CONFIG_MAX_BASEPRI): "memory");

    return flags;
}

static inline void
gyros_interrupt_restore(unsigned long flags)
{
    __asm__ __volatile__(
        "msr    basepri, %0\n\t"
        :: "r" (flags) : "memory");
}

static inline int
gyros_in_interrupt(void)
{
    unsigned long ipsr;

    __asm__ __volatile__(
        "mrs    %0, ipsr\n\t"
        : "=r" (ipsr) :: "memory");

    return ipsr != 0;
}

/* Reschedule, i.e. make sure the right task is running. */
static inline void
gyros__reschedule(void)
{
    *(unsigned long*)0xe000ed04 = 1U << 28;
}

/* Reschedule, i.e. make sure the right task is running. */
static inline void
gyros__tick_reschedule(void)
{
    gyros__reschedule();
}

void gyros_target_enable_irq(int irq, unsigned prio);

void gyros_target_disable_irq(int irq);

void gyros_target_pend_irq(int irq);

#endif
