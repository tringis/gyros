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
#ifndef INCLUDED__gyros_sem_h__200808281430
#define INCLUDED__gyros_sem_h__200808281430

/** \file sem.h
 * \brief Semaphores.
 */

#include <limits.h>

#include <gyros/private/debug.h>
#include <gyros/task.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#if GYROS_DEBUG
#define GYROS_SEM_DEBUG_MAGIC           0xe111100a
#define GYROS_SEM_DEBUG_INITIALIZER     GYROS_SEM_DEBUG_MAGIC,
#else
#define GYROS_SEM_DEBUG_INITIALIZER
#endif
#endif

/** Define a ready to use (initialized) counting semaphore by the
  * specified @a name with start value @a start_value. */
#define GYROS_SEM_DEFINE(name, start_value) \
    gyros_sem_t name = { GYROS_SEM_DEBUG_INITIALIZER                    \
                         start_value, UINT_MAX,                         \
                         GYROS__LIST_INITALIZER(name.task_list) }

/** Define a ready to use (initialized) binary semaphore by the
  * specified @a name. */
#define GYROS_SEM_DEFINE_BINARY(name) \
    gyros_sem_t name = { GYROS_SEM_DEBUG_INITIALIZER                    \
                         0, 1, GYROS__LIST_INITALIZER(name.task_list) }


/** Semaphore (sem). */
typedef struct gyros_sem
{
#if GYROS_DEBUG
    unsigned debug_magic; /**< \internal */
#endif

    unsigned value; /**< \internal */
    unsigned max_value; /**< \internal */
    struct gyros__list_node task_list; /**< \internal */
} gyros_sem_t;

/** Initialize the semaphore @a s to be a counting semaphore with
 * start value @a start_value.
  *
  * \param s            Semaphore struct pointer.
  * \param start_value  Start value.
  */
void gyros_sem_init(gyros_sem_t *s, int start_value);

/** Initialize the semaphore @a s to be an unsignalled binary
  * semaphore.
  *
  * \param s            Semaphore struct pointer.
  */
void gyros_sem_init_binary(gyros_sem_t *s);

/** Wait for the semaphore @a s to be signalled.
  *
  * \param s            Semaphore struct pointer.
  */
void gyros_sem_wait(gyros_sem_t *s);

/** Wait for the semaphore @a s to be signalled, or until the absolute
 * time @a timeout has passed.
  *
  * \param s            Semaphore struct pointer.
  * \param timeout      Timeout.  See gyros_time().
  * \return             Non-zero if the semaphore was signalled, or
  *                     zero if @a timeout was reached.
  */
int gyros_sem_timedwait(gyros_sem_t *s, gyros_abstime_t timeout);

/** Signal the semaphore @a s.
  *
  * \param s            Semaphore struct pointer.
  */
void gyros_sem_signal(gyros_sem_t *s);

#endif
