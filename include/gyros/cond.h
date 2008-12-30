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
#ifndef INCLUDED__gyros_cond_h__200808272231
#define INCLUDED__gyros_cond_h__200808272231

/** \file cond.h
 * \brief Condition variable.
 */

#include <gyros/mutex.h>

/** Condition variable (cond). */
typedef struct gyros_cond {
#if GYROS_DEBUG
    unsigned debug_magic; /**< \internal */
#endif

    struct gyros__list_node task_list; /**< \internal */
} gyros_cond_t;

/** Initialize the condition variable @a c.
  *
  * \param c            Condition variable struct pointer.
  */
void gyros_cond_init(gyros_cond_t *c);

/** Wait for the condition variable @a c.  The mutex @a is unlocked
  * atomically before waiting, and locked again before returning.
  *
  * \param c            Condition variable struct pointer.
  * \param m            Mutex struct pointer.
  */
void gyros_cond_wait(gyros_cond_t *c, gyros_mutex_t *m);

/** Wait for the condition variable @a c, or until the absolute time
  * @a timeout has passed.  The mutex @a is unlocked atomically before
  * waiting, and locked again before returning.
  *
  * \param c            Condition variable struct pointer.
  * \param m            Mutex struct pointer.
  * \param timeout      Timeout.  See gyros_time().
  * \return             Non-zero if the semaphore was signalled, or
  *                     zero if @a timeout was reached.
  */
int gyros_cond_timedwait(gyros_cond_t *c, gyros_mutex_t *m,
                         gyros_abstime_t timeout);

/** Signal one task waiting for the condition variable @a c.
  *
  * \param c            Condition variable struct pointer.
  */
void gyros_cond_signal_one(gyros_cond_t *c);

/** Signal all tasks waiting for the condition variable @a c.
  *
  * \param c            Condition variable struct pointer.
  */
void gyros_cond_signal_all(gyros_cond_t *c);

#endif
