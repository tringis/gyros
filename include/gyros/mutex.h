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

/** @file mutex.h
  * \brief Mutual exclusion (mutex).
  *
  * A mutex must be initialized before use, either using
  * GYROS_MUTEX_INITVAL() when defining the mutex, or using
  * gyros_mutex_init().
  *
  * Mutexes are not recursive, so calling gyros_mutex_lock() twice
  * from the same task (without calling gyros_mutex_unlock()
  * inbetween) will result in a deadlock.
  */

#include <gyros/target/config.h>
#include <gyros/task.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#if GYROS_CONFIG_DEBUG
#define GYROS_MUTEX_DEBUG_MAGIC         0xe398123d
#define GYROS_MUTEX_DEBUG_INITIALIZER   GYROS_MUTEX_DEBUG_MAGIC,
#else
#define GYROS_MUTEX_DEBUG_INITIALIZER
#endif
#endif

/** Initialization value for a mutex by the specified @a name.  When a
  * mutex is initialized using this value, gyros_mutex_init() does not
  * need to be called.  Example:
  *
  * \code
  * gyros_mutex_t my_mutex = GYROS_MUTEX_INITVAL(my_mutex);
  * \endcode
  */
#define GYROS_MUTEX_INITVAL(name) \
    { GYROS_MUTEX_DEBUG_INITIALIZER                                     \
      (gyros_task_t*)0, 0,                                              \
      GYROS__LIST_INITVAL(name.task_list) }

/** \brief Mutual exclusion (mutex) object. */
typedef struct
{
#if GYROS_CONFIG_DEBUG
    unsigned debug_magic; /**< \internal */
#endif

    gyros_task_t *owner; /**< \internal */
    unsigned short owner_priority; /**< \internal */
    struct gyros__list_node task_list; /**< \internal */
} gyros_mutex_t;

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
int gyros_mutex_try_lock(gyros_mutex_t *m);

/** Lock @a m.
  *
  * \param m            Mutex struct pointer.
  */
void gyros_mutex_lock(gyros_mutex_t *m);

/** Unlock @a m.
  *
  * \param m            Mutex struct pointer.
  */
void gyros_mutex_unlock(gyros_mutex_t *m);

#endif
