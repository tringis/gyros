/**************************************************************************
 * Copyright (c) 2002-2008, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDED__gyros_rwlock_h__200812121950
#define INCLUDED__gyros_rwlock_h__200812121950

/** \file rwlock.h
 * \brief Read/write locks.
 *
 * A read/write lock (rwlock) allows multiple read locks
 * simultaneously, but write locks are exclusive, locking out all
 * other readers and writers.
 */

#include <gyros/task.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#if GYROS_DEBUG
#define GYROS_RWLOCK_DEBUG_MAGIC         0xe151110d
#define GYROS_RWLOCK_DEBUG_INITIALIZER   GYROS_RWLOCK_DEBUG_MAGIC,
#else
#define GYROS_RWLOCK_DEBUG_INITIALIZER
#endif
#endif

/** Define a ready to use (initialized) read/write lock by the
  * specified @a name. */
#define GYROS_RWLOCK_DEFINE(name) \
    gyros_rwlock_t name = { GYROS_RWLOCK_DEBUG_INITIALIZER                \
                            (gyros_task_t*)0, 0,                          \
                            GYROS__LIST_INITALIZER(name.rd_task_list),    \
                            GYROS__LIST_INITALIZER(name.wr_task_list) }

/** Read/write lock (rwlock). */
typedef struct gyros_rwlock
{
#if GYROS_DEBUG
    unsigned debug_magic; /**< \internal */
#endif

    gyros_task_t *writer; /**< \internal */
    int readers; /**< \internal */

    struct gyros__list_node rd_task_list; /**< \internal */
    struct gyros__list_node wr_task_list; /**< \internal */
} gyros_rwlock_t;

/** Initialize the read/write lock @a rwlock.
  *
  * \param rwlock       Read/write lock struct pointer.
  */
void gyros_rwlock_init(gyros_rwlock_t *rwlock);

/** Aquire a reader lock on @a rwlock.
  *
  * \param rwlock       Read/write lock struct pointer.
  */
void gyros_rwlock_rdlock(gyros_rwlock_t *rwlock);

/** Try to aquire a reader lock on @a rwlock.
  *
  * \param rwlock       Read/write lock struct pointer.
  */
int gyros_rwlock_tryrdlock(gyros_rwlock_t *rwlock);

/** Aquire a reader lock on @a rwlock, unless @a timeout is passed
  * before the lock can be aquired without blocking.
  *
  * \param rwlock       Read/write lock struct pointer.
  * \param timeout      Timeout.  See gyros_time().
  * \return             Non-zero if the semaphore was signalled, or
  *                     zero if @a timeout was reached.
  */
int gyros_rwlock_timedrdlock(gyros_rwlock_t *rwlock, gyros_abstime_t timeout);

/** Aquire a writer lock on @a rwlock.
  *
  * \param rwlock       Read/write lock struct pointer.
  */
void gyros_rwlock_wrlock(gyros_rwlock_t *rwlock);

/** Try to aquire a writer lock on @a rwlock.
  *
  * \param rwlock       Read/write lock struct pointer.
  */
int gyros_rwlock_trywrlock(gyros_rwlock_t *rwlock);

/** Aquire a writer lock on @a rwlock, unless @a timeout is passed
  * before the lock can be aquired without blocking.
  *
  * \param rwlock       Read/write lock struct pointer.
  * \param timeout      Timeout.  See gyros_time().
  * \return             Non-zero if the semaphore was signalled, or
  *                     zero if @a timeout was reached.
  */
int gyros_rwlock_timedwrlock(gyros_rwlock_t *rwlock, gyros_abstime_t timeout);

/** Unlock the read/write lock @a rwlock.
  *
  * \param rwlock       Read/write lock struct pointer.
  */
void gyros_rwlock_unlock(gyros_rwlock_t *rwlock);

#endif
