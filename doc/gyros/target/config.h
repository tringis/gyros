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
#ifndef INCLUDED__gyros_private_defconfig_h__200901021029
#define INCLUDED__gyros_private_defconfig_h__200901021029

/** \defgroup config_group GyrOS configuration
  *
  * A few aspects of GyrOS can be configured as build time using the
  * include file \c gyros/target/config.h.  This file is port specific
  * and should be copied from the port's examples directory,
  * e.g. @c port/at91sam7s/examples/include/gyros/target/config.h.
  */

/*@{*/

/** \file
  * \brief GyrOS configuration.
  * \copydoc config_group
  */

/** If defined to a non-zero value, this variable enables support for
  * the \ref gyros_set_context_hook "context switch hook". */
#define GYROS_CONFIG_CONTEXT_HOOK 0

/** If defined to a non-zero value, this variable enables support for
  * implementing a custom idle loop.  When enabled, the gyros_start()
  * function returns instead of performing the idle loop internally.
  *
  * \warning
  * Note that the custom idle loop must not call functions that may
  * block. */
#define GYROS_CONFIG_CUSTOM_IDLE_LOOP 0

/** If defined to a non-zero value, this variable enables \ref
  * debug_group "debugging support".  The cost of enabling debugging
  * support is higher memory usage and a small performance penalty. */
#define GYROS_CONFIG_DEBUG 1

/** If defined to a non-zero value, this variable enables support for
  * the \ref gyros_set_irq_hook "IRQ hook". */
#define GYROS_CONFIG_IRQ_HOOK 0

/** If defined to a non-zero value, this variable enables support for
  * gyros_task_iterate() at the cost of a few extra bytes memory usage
  * per task. */
#define GYROS_CONFIG_ITERATE 1

/** If defined to a non-zero value, this variable enables support for
  * gyros_task_stack_used() at the cost of a performance hit when
  * creating a new task.  The reason for the performance hit is that
  * the task's stack must be filled with a known data pattern. */
#define GYROS_CONFIG_STACK_USED 1

/** Base data type for \ref gyros_abstime_t and \ref
  * gyros_reltime_t. */
#define GYROS_CONFIG_TIME_TYPE long long

/** If defined to a non-zero value, this variable enables support for
  * \ref timer_group "timers". */
#define GYROS_CONFIG_TIMER 1

/** If defined to a non-zero value, this variable enables support for
  * gyros_task_wait() and gyros_task_wait_until() at the cost of a few
  * extra bytes memory usage per task. */
#define GYROS_CONFIG_WAIT 1

/** If defined to a non-zero value, this variable enables support for
  * \ref trace_group "tracing". */
#define GYROS_CONFIG_TRACE 0

/*@}*/

#endif
