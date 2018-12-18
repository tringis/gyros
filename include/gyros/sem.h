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
#ifndef INCLUDED__gyros_sem_h__200808281430
#define INCLUDED__gyros_sem_h__200808281430

/** \file
  * \brief Semaphores.
  * \details Header file for \ref sem_group.
  *
  * \defgroup sem_group Semaphores
  *
  * A semaphore must be initialized before use, either using
  * GYROS_SEM_INITVAL() or GYROS_BINARY_SEM_INITVAL() when defining
  * the semaphore, or using gyros_sem_init().
  *
  * @{
  */

#include <limits.h>

#include <gyros/config.h>
#include <gyros/private/debug.h>
#include <gyros/task.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_SEM_DEBUG_MAGIC                 ((unsigned)0xe111100a)
#endif

/** Initialization value for a counting semaphore by the specified @a
  * name and start value @a start_value.  When a semaphore is
  * initialized using this value, gyros_sem_init() does not need to be
  * called.  Example:
  *
  * \code
  * gyros_sem_t my_sem = GYROS_SEM_INITVAL(my_sem, 0);
  * \endcode
  *
  * \param name         Name of the semaphore variable.
  * \param start_value  Start value.
  */
#define GYROS_SEM_INITVAL(name, start_value)            \
    { GYROS_DEBUG_INFO(GYROS_SEM_DEBUG_MAGIC, #name),   \
      start_value, UINT_MAX,                            \
      GYROS__LIST_INITVAL((name).task_list) }

/** Initialization value for a binary semaphore by the specified @a
  * name.  When a semaphore is initialized using this value,
  * gyros_sem_init_binary() does not need to be called.  Example:
  *
  * \code
  * gyros_sem_t my_sem = GYROS_BINARY_SEM_INITVAL(my_sem);
  * \endcode
  *
  * \param name         Name of the semaphore variable.
  */
#define GYROS_BINARY_SEM_INITVAL(name) \
    { GYROS_DEBUG_INFO(GYROS_SEM_DEBUG_MAGIC, #name),   \
      0, 1, GYROS__LIST_INITVAL((name).task_list) }


/** \brief Semaphore (sem) object. */
typedef struct
{
    struct gyros_debug_info debug_info; /**< Debug info. */

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
int gyros_sem_wait_until(gyros_sem_t *s, gyros_abstime_t timeout);

/** Signal the semaphore @a s.  May be called from interrupt context.
  *
  * \param s            Semaphore struct pointer.
  */
void gyros_sem_signal(gyros_sem_t *s);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
