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
#ifndef INCLUDED__gyros_armv7_m_defconfig_h__201002062209
#define INCLUDED__gyros_armv7_m_defconfig_h__201002062209

#ifndef GYROS_CONFIG_DYNTICK
#define GYROS_CONFIG_DYNTICK                   0
#endif

#ifndef GYROS_CONFIG_EXCEPTION_STACK_SIZE
#define GYROS_CONFIG_EXCEPTION_STACK_SIZE      512
#endif

#ifndef GYROS_CONFIG_IDLE_WFI
#define GYROS_CONFIG_IDLE_WFI                  1
#endif

/* The GYROS_CONFIG_MAX_IRQ_PRIORITY configures the highest exception
 * priority that may call GyrOS functions.
 *
 * The default setting of zero means that all exceptions are allowed
 * to call any GyrOS function (that is interrupts safe of course).  It
 * also means that GyrOS will temprorarily disable all interrupts for
 * short periods of time.  Reducing the priority (by increasing the
 * value) allows the system to run interrupt service routines at a
 * higher priority than GyrOS to achieve best possible interrupt
 * latency, at the expense that the ISR must not call GyrOS
 * functions. */
#ifndef GYROS_CONFIG_MAX_IRQ_PRIORITY
#define GYROS_CONFIG_MAX_IRQ_PRIORITY          0
#endif

#ifndef GYROS_CONFIG_MIN_IRQ_PRIORITY
#define GYROS_CONFIG_MIN_IRQ_PRIORITY          255
#endif

#ifndef GYROS_CONFIG_SYSTICK_PRIORITY
#define GYROS_CONFIG_SYSTICK_PRIORITY          0xa0
#endif


#if !GYROS_CONFIG_DYNTICK && !defined(GYROS_CONFIG_CORE_HZ)
#error GYROS_CONFIG_CORE_HZ not defined
#endif

#if !GYROS_CONFIG_DYNTICK && !defined(GYROS_CONFIG_HZ)
#define GYROS_CONFIG_HZ                        1000
#endif

#endif
