/**************************************************************************
 * Copyright (c) 2002-2022, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDED__gyros_alarm_clock_h__202206161103
#define INCLUDED__gyros_alarm_clock_h__202206161103

/** \file
  * \brief Alarm clocks.
  * \details Header file for \ref alarm_clock_group.
  *
  * \defgroup alarm_clock_group Alarm Clocks
  *
  * A alarm clock must be initialized before use, either using
  * GYROS_ALARM_CLOCK_INITVAL() or GYROS_BINARY_ALARM_CLOCK_INITVAL() when defining
  * the alarm clock, or using gyros_alarm_clock_init().
  *
  * @{
  */

#include <limits.h>
#include <stdbool.h>

#include <gyros-config.h>
#include <gyros/private/debug.h>
#include <gyros/task.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_ALARM_CLOCK_DEBUG_MAGIC         ((unsigned)0xfafaefea)
#endif

/** Initialization value for a counting alarm clock by the specified @a
  * name and start value @a start_value.  When a alarm clock is
  * initialized using this value, gyros_alarm_clock_init() does not need to be
  * called.  Example:
  *
  * \code
  * gyros_alarm_clock_t my_alarm_clock = GYROS_ALARM_CLOCK_INITVAL(my_alarm_clock, 0);
  * \endcode
  *
  * \param name         Name of the alarm clock variable.
  */
#define GYROS_ALARM_CLOCK_INITVAL(name)                         \
    { GYROS_DEBUG_INFO(GYROS_ALARM_CLOCK_DEBUG_MAGIC, #name),   \
      false, 0,                                                 \
      GYROS__LIST_INITVAL((name).task_list) }

/** Define and initialize a alarm clock @a name.  Example:
  *
  * \code
  * GYROS_DEFINE_ALARM_CLOCK(my_alarm_clock);
  * static GYROS_DEFINE_ALARM_CLOCK(my_static_alarm_clock);
  * \endcode
  *
  * \param name         Name of the alarm clock.
  */
#define GYROS_DEFINE_ALARM_CLOCK(name)     \
    gyros_alarm_clock_t name = GYROS_ALARM_CLOCK_INITVAL(name)


/** \brief Alarm Clock (alarm_clock) object. */
typedef struct
{
    struct gyros_debug_info debug_info; /**< Debug info. */

    bool armed; /**< \internal */
    gyros_abstime_t wakeup_time; /**< \internal */
    struct gyros__list_node task_list; /**< \internal */
} gyros_alarm_clock_t;

/** Initialize the alarm clock @a a.
  *
  * \param a            Alarm Clock struct pointer.
  */
void gyros_alarm_clock_init(gyros_alarm_clock_t *a);

/** Wait for the alarm clock @a a to be signalled.
  *
  * \param a            Alarm Clock struct pointer.
  */
void gyros_alarm_clock_wait(gyros_alarm_clock_t *a);

/** Signal the alarm clock @a a.  May be called from interrupt context.
  *
  * \param a            Alarm Clock struct pointer.
  * \param wakeup_time  Wakeup time.
  */
void gyros_alarm_clock_set(gyros_alarm_clock_t *a, gyros_abstime_t wakeup_time);

/** Signal the alarm clock @a a.  May be called from interrupt context.
  *
  * \param a            Alarm Clock struct pointer.
  */
void gyros_alarm_clock_reset(gyros_alarm_clock_t *a);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
