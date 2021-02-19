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

#include <stdbool.h>

#include <gyros-config.h>
#include <gyros/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

GYROS_ALWAYS_INLINE unsigned long
gyros__get_primask(void)
{
    unsigned long primask;

    __asm__ __volatile__(
        "mrs    %0, primask\n\t"
        : "=&r" (primask) :: "memory");

    return primask;
}

GYROS_ALWAYS_INLINE void
gyros__set_primask(unsigned long primask)
{
    __asm__ __volatile__(
        "msr    primask, %0\n\t"
        :: "r" (primask): "memory");
}

GYROS_ALWAYS_INLINE unsigned long
gyros__get_faultmask(void)
{
    unsigned long faultmask;

    __asm__ __volatile__(
        "mrs    %0, faultmask\n\t"
        : "=&r" (faultmask) :: "memory");

    return faultmask;
}

GYROS_ALWAYS_INLINE unsigned long
gyros__get_basepri(void)
{
    unsigned long basepri;

    __asm__ __volatile__(
        "mrs    %0, basepri\n\t"
        : "=&r" (basepri) :: "memory");

    return basepri;
}

GYROS_ALWAYS_INLINE void
gyros__set_basepri(unsigned long basepri)
{
    __asm__ __volatile__(
        "msr    basepri, %0\n\t"
        :: "r" (basepri): "memory");
}

GYROS_ALWAYS_INLINE void
gyros__set_basepri_max(unsigned long basepri_max)
{
    __asm__ __volatile__(
        "msr    basepri_max, %0\n\t"
        :: "r" (basepri_max): "memory");
}

GYROS_ALWAYS_INLINE unsigned long
gyros_interrupt_disable(void)
{
    unsigned long basepri = gyros__get_basepri();

    gyros__set_basepri_max(GYROS_CONFIG_MAX_IRQ_PRIORITY);

    return basepri;
}

GYROS_ALWAYS_INLINE void
gyros_interrupt_restore(unsigned long flags)
{
    gyros__set_basepri(flags);
}

GYROS_ALWAYS_INLINE bool
gyros_in_interrupt(void)
{
    unsigned long ipsr;

    __asm__ __volatile__(
        "mrs    %0, ipsr\n\t"
        : "=r" (ipsr) :: "memory");

    return ipsr != 0;
}

GYROS_ALWAYS_INLINE bool
gyros_interrupts_disabled(void)
{
    return ((gyros__get_primask() & 1) == 1 ||
            (gyros__get_faultmask() & 1) == 1 ||
            gyros__get_basepri() != 0);
}

/* Reschedule, i.e. make sure the right task is running. */
GYROS_ALWAYS_INLINE void
gyros__reschedule(void)
{
    *(volatile unsigned long*)0xe000ed04 = 1U << 28; /* pendSV */
}

/* Reschedule, i.e. make sure the right task is running. */
GYROS_ALWAYS_INLINE void
gyros__tick_reschedule(void)
{
    gyros__reschedule();
}

void gyros_target_enable_irq(int irq, unsigned prio);

void gyros_target_disable_irq(int irq);

void gyros_target_pend_irq(int irq);

#ifdef __cplusplus
}
#endif

#endif
