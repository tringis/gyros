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
#ifndef INCLUDED__gyros_mutex_h__200808272211
#define INCLUDED__gyros_mutex_h__200808272211

/** \file
  * \brief Mutual exclusion (mutex).
  * \details Header file for \ref mutex_group.
  *
  * \defgroup mutex_group Mutual exclusion
  *
  * A mutex is a synchronization object which is used to grant
  * exclusive access to a shared resource.  If one task has locked the
  * mutex, any other task that tries to lock the mutex will block
  * until the mutex is unlocked.
  *
  * To combat priority inversion, the mutex implementation in GyrOS
  * supports priority inheritence, which means that if a high priority
  * task blocks on a mutex owned by a lower priority task, the lower
  * priority task inherits the high priority until it unlocks the
  * mutes.  This prevents the high priority task from getting starved
  * by medium priority tasks which starve the low priority task.
  *
  * A mutex must be initialized before use, either using
  * GYROS_MUTEX_INITVAL() when defining the mutex, or using
  * gyros_mutex_init().
  *
  * Mutexes are not recursive, so calling gyros_mutex_lock() twice
  * from the same task (without calling gyros_mutex_unlock()
  * inbetween) will result in a deadlock.  This error condition is
  * detected if \ref GYROS_CONFIG_DEBUG is enabled.
  *
  * @{
  */

#include <stddef.h>

#include <gyros/atomic.h>
#include <gyros/compiler.h>
#include <gyros/private/debug.h>
#include <gyros/private/state.h>
#include <gyros/task.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_MUTEX_DEBUG_MAGIC               ((unsigned)0xe398123d)
#endif

/** Initialization value for a mutex by the specified @a name.  When a
  * mutex is initialized using this value, gyros_mutex_init() does not
  * need to be called.  Example:
  *
  * \code
  * gyros_mutex_t my_mutex = GYROS_MUTEX_INITVAL(my_mutex);
  * \endcode
  *
  * \param name         Name of the mutex variable.
  */
#define GYROS_MUTEX_INITVAL(name) \
    { GYROS_DEBUG_INFO(GYROS_MUTEX_DEBUG_MAGIC, #name),                 \
      (gyros_task_t*)0,                                                 \
      GYROS__LIST_INITVAL((name).task_list) }

/** \brief Mutual exclusion (mutex) object. */
typedef struct
{
    struct gyros_debug_info debug_info; /**< Debug info. */

    gyros_task_t *owner; /**< \internal */
    struct gyros__list_node task_list; /**< \internal */
} gyros_mutex_t;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
int gyros__mutex_try_lock_slow(gyros_mutex_t *m);
void gyros__mutex_lock_slow(gyros_mutex_t *m);
void gyros__mutex_unlock_slow(gyros_mutex_t *m, int reschedule);
#endif

/** Initialize the mutex @a m.
  *
  * \param m            Mutex struct pointer.
  */
void gyros_mutex_init(gyros_mutex_t *m);

/** Try locking @a m.
  *
  * \param m            Mutex struct pointer.
  * \return             Non-zero if @a m was locked, else zero.
  */
static inline int gyros_mutex_try_lock(gyros_mutex_t *m)
{
#if defined(GYROS_HAS_LDREX_STREX) && !GYROS_CONFIG_DEBUG
    extern gyros_t gyros;
    while (GYROS_LIKELY(gyros_ldrex_p(&m->owner) == NULL))
    {
        if (GYROS_LIKELY(gyros_strex_p(&m->owner, gyros.current)))
            return 1;
    }
#endif
    return gyros__mutex_try_lock_slow(m);
}

/** Lock @a m.
  *
  * \param m            Mutex struct pointer.
  */
static inline void gyros_mutex_lock(gyros_mutex_t *m)
{
#if defined(GYROS_HAS_LDREX_STREX) && !GYROS_CONFIG_DEBUG
    extern gyros_t gyros;
    while (GYROS_LIKELY(gyros_ldrex_p(&m->owner) == NULL))
    {
        if (GYROS_LIKELY(gyros_strex_p(&m->owner, gyros.current)))
            return;
    }
#endif
    gyros__mutex_lock_slow(m);
}

/** Unlock @a m.
  *
  * \param m            Mutex struct pointer.
  */
static inline void gyros_mutex_unlock(gyros_mutex_t *m)
{
#if defined(GYROS_HAS_LDREX_STREX) && !GYROS_CONFIG_DEBUG
    extern gyros_t gyros;
    while (GYROS_LIKELY(gyros_ldrex_p(&m->task_list.next) == &m->task_list))
    {
        if (GYROS_LIKELY(gyros_strex_p(&m->owner, NULL)))
            return;
    }
#endif
    gyros__mutex_unlock_slow(m, 1);
}

/*@}*/

#endif
