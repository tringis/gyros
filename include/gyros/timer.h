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
#ifndef INCLUDE__gyros_timer_h__200905041911
#define INCLUDE__gyros_timer_h__200905041911

/** \defgroup timer_group Timer management
  *
  * Timers are objects that generate callbacks at specified times, and
  * may be either periodic or one shot.  Timers are called from
  * interrupt context.
  */
/*@{*/

/** \file
  * \brief Timer management.
  * \copydoc timer_group
  */

#include <gyros/private/debug.h>
#include <gyros/private/list.h>
#include <gyros/time.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#if GYROS_CONFIG_DEBUG
#define GYROS_TIMER_DEBUG_MAGIC           ((unsigned)0xe111100a)
#define GYROS_TIMER_DEBUG_INITIALIZER     GYROS_TIMER_DEBUG_MAGIC,(char*)0,
#else
#define GYROS_TIMER_DEBUG_INITIALIZER
#endif
#endif

/** \brief Timer object. */
typedef struct gyros_timer
{
#if GYROS_CONFIG_DEBUG
    unsigned debug_magic; /**< \internal */
    /** Name of the timer set by gyros_timer_name(), else @c NULL. */
    const char *name;
#endif

    struct gyros__list_node list_node; /**< \internal */

    gyros_abstime_t timeout; /**< \internal */
    gyros_reltime_t period; /**< \internal */

    void (*callback)(struct gyros_timer *timer,
                     gyros_abstime_t now,
                     void *arg); /**< \internal */
    void *callback_arg; /**< \internal */
} gyros_timer_t;

/** Initialize the timer to call a callback function upon activation.
  *
  * \param timer        Timer struct pointer.
  * \param callback     Address of callback.  Must not be @c NULL.
  * \param callback_arg Argument to callback function.
  */
void gyros_timer_init(gyros_timer_t *timer,
                      void (*callback)(struct gyros_timer *timer,
                                       gyros_abstime_t now,
                                       void *arg),
                      void *callback_arg);

/** Name the @a timer to @a name.  The name is only used when
  * GYROS_CONFIG_DEBUG is true.
  *
  * \param timer        Timer struct pointer.
  * \param name         Timer name.  Note that only the pointer to the
  *                     name is stored, so the pointer must remain valid
  *                     for the lifetime of the timer.
  */
void gyros_timer_name(gyros_timer_t *timer, const char *name);

/** Set @a timer to activate at @a time.  The timer is rescheduled
  * if it was already set.
  *
  * \param timer        Timer struct pointer.
  * \param time         Activation time.
  */
void gyros_timer_set(gyros_timer_t *timer, gyros_abstime_t time);

/** Set @a timer to activate at @a time.  The timer is rescheduled
  * if it was already set.
  *
  * \param timer        Timer struct pointer.
  * \param period       Activation period.
  */
void gyros_timer_set_periodic(gyros_timer_t *timer, gyros_reltime_t period);

/** Clear timer.  It is valid to call this function even if the 
  *
  * \param timer        Timer struct pointer.
  */
void gyros_timer_clear(gyros_timer_t *timer);

/*@}*/

#endif
