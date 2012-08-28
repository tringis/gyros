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

/** \file
  * \brief Timer management.
  * \details Header file for \ref timer_group.
  *
  * \defgroup timer_group Timer management
  *
  * Timers are objects that generate callbacks at specified times, and
  * may be either periodic or one shot.  Timers are called from
  * interrupt context.
  *
  * @{
  */

#include <gyros/private/debug.h>
#include <gyros/private/list.h>
#include <gyros/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_TIMER_DEBUG_MAGIC           ((unsigned)0xe111100a)
#endif

/** Initialization value for a timer by the specified @a name,
  * callback function and argument.  When a timer is initialized using
  * this value, gyros_timer_init() does not need to be called.
  * Example:
  *
  * \code
  * void my_callback(struct gyros_timer *timer, gyros_abstime_t now, void *arg);
  *
  * gyros_timer_t my_timer = GYROS_TIMER_INITVAL(my_timer, my_callback, NULL);
  * \endcode
  *
  * \param name         Name of the timer variable.
  * \param callback     Callback function.
  * \param callback_arg Argument to callback function.
  */
#define GYROS_TIMER_INITVAL(name, callback, callback_arg)               \
    { GYROS_DEBUG_INFO(GYROS_TIMER_DEBUG_MAGIC, #name),                 \
      GYROS__LIST_INITVAL((name).list_node),                            \
      0, 0, (callback), (callback_arg) }

/** \brief Timer object. */
typedef struct gyros_timer
{
    struct gyros_debug_info debug_info; /**< Debug info. */

    struct gyros__list_node list_node; /**< \internal */

    gyros_abstime_t timeout; /**< \internal */
    gyros_reltime_t period; /**< \internal */

    void (*callback)(struct gyros_timer *timer,
                     gyros_abstime_t now,
                     void *arg); /**< \internal */
    void *callback_arg; /**< \internal */
} gyros_timer_t;

/** Initialize the timer to call a callback function upon activation.
  * May be called from interrupt context.
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

/** Set @a timer to activate at @a time.  The timer is rescheduled
  * if it was already set.  May be called from interrupt context.
  *
  * \param timer        Timer struct pointer.
  * \param time         Activation time.
  * \param period       Activation period for a periodic timer,
                        or zero for a one-shot timer.
  */
void gyros_timer_start(gyros_timer_t *timer,
                       gyros_abstime_t time,
                       gyros_reltime_t period);

/** Clear timer.  It is valid to call this function even if the timer
  * is not active.  May be called from interrupt context.
  *
  * \param timer        Timer struct pointer.
  */
void gyros_timer_stop(gyros_timer_t *timer);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
