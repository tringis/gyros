/**************************************************************************
 * Copyright (c) 2002-2018, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDED__gyros_event_group_h__201810041823
#define INCLUDED__gyros_event_group_h__201810041823

/** \file
  * \brief Event group.
  * \details Header file for \ref event_group_group.
  *
  * \defgroup event_group_group Events
  *
  * An event group must be initialized before use using
  * GYROS_EVENT_GROUP_INITVAL() when defining the event group,
  * or using gyros_event_group_init().
  *
  * @{
  */

#include <limits.h>
#include <stdbool.h>

#include <gyros/config.h>
#include <gyros/private/debug.h>
#include <gyros/task.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_EVENT_GROUP_DEBUG_MAGIC           ((unsigned)0xe151120a)
#endif

/** Initialization value for a event group by the specified @a
  * name.  Example:
  *
  * \code
  * gyros_event_t my_event = GYROS_EVENT_GROUP_INITVAL(my_event);
  * \endcode
  *
  * \param name         Name of the event group variable.
  */
#define GYROS_EVENT_GROUP_INITVAL(name)                       \
    { GYROS_DEBUG_INFO(GYROS_EVENT_GROUP_DEBUG_MAGIC, #name),   \
      0u,                            \
      GYROS__LIST_INITVAL((name).task_list) }


/** \brief event group object. */
typedef struct
{
    struct gyros_debug_info debug_info; /**< Debug info. */

    unsigned events; /**< \internal */
    struct gyros__list_node task_list; /**< \internal */
} gyros_event_group_t;

/** Initialize the event group.
  *
  * \param e            event group struct pointer.
  */
void gyros_event_group_init(gyros_event_group_t *e);

/** Wait for any of the @a events in the event group @a e to be set.
  *
  * \param e            event group struct pointer.
  * \param wait_events  Events to wait for.
  * \param wait_all     Wait for all events.
  * \param events       Returned events.
  */
void gyros_event_group_get(gyros_event_group_t *e,
                           unsigned wait_events,
                           bool wait_all,
                           unsigned *events);

/** Wait for any of the @a events in the event group @a e to be set, or
  * until the absolute time @a timeout has passed.
  *
  * \param e            event group struct pointer.
  * \param wait_events  Events to wait for.
  * \param wait_all     Wait for all events.
  * \param events       Returned events.
  * \param timeout      Timeout.  See gyros_time().
  * \return             Non-zero if the semaphore was signalled, or
  *                     zero if @a timeout was reached.
  */
bool gyros_event_group_get_until(gyros_event_group_t *e,
                                 unsigned wait_events,
                                 bool wait_all,
                                 unsigned *events,
                                 gyros_abstime_t timeout);

/** Set @a events in the event @a e.  May be called from interrupt
  * context.
  *
  * \param e            Semaphore struct pointer.
  * \param events       Events to set.
  */
void gyros_event_group_set(gyros_event_group_t *e, unsigned events);

/** Clear all events in the event group.  May be called from interrupt
  * context.
  *
  * \param e            event group struct pointer.
  * \param events       Events to clear.
  */
void gyros_event_group_clear(gyros_event_group_t *e, unsigned events);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
