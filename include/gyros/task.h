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
#ifndef INCLUDED__gyros_task_h__200206071335
#define INCLUDED__gyros_task_h__200206071335

/** \file task.h
 * \brief Task management.
 */

#include <gyros/target/config.h>
#include <gyros/target/context.h>
#include <gyros/private/debug.h>
#include <gyros/private/list.h>
#include <gyros/time.h>

/** The structure representing a GyrOS task.  Most members of this
  * stucture are for internal GyrOS use only, and are not meant to be
  * accessed directly. */
typedef struct
{
    struct gyros__context context; /**< \internal */

    struct gyros__list_node main_list; /**< \internal */

    struct gyros__list_node timeout_list; /**< \internal */
    gyros_abstime_t timeout; /**< \internal */
    unsigned char timed_out; /**< \internal */

    unsigned char raised_priority; /**< \internal */
    unsigned short priority; /**< \internal */

#if GYROS_CONFIG_ITERATE
    struct gyros__list_node task_list; /**< \internal */
#endif

    const char *name; /**< Task name. */
    void *stack; /**< Task stack base address. */
    int stack_size; /**< Task stack size in bytes. */

#if GYROS_CONFIG_DEBUG
    unsigned debug_magic; /**< \internal */
    const char *debug_state; /**< String representing the task state. */
    /** Pointer to the object related to the currect task state,
      * e.g. a pointer to the semaphore if debug_state is
      * "sem_wait". */
    void *debug_object;
#endif
} gyros_task_t;

/** Start GyrOS.  This call creates the idle task, enables interrupts
  * and loops forever as the idle task.  This idle task has priority
  * zero and is always running.
  */
void gyros_start(void) __attribute__((__noreturn__));

/** Create a new task.
  *
  * \param task         Task struct pointer.
  * \param name         Task name.  The pointer is stored, not the
  *                     string data, so the pointer must remain valid
  *                     during the lifetime of the process.
  * \param entry        Address to task main function.  The task will
  *                     be deleted when exiting the function.
  * \param arg          Entry point argument.
  * \param stack        Stack start.
  * \param stack_size   Stack size in bytes.
  * \param priority     Priority.  Should be greater than zero.
  */
void gyros_task_create(gyros_task_t *task,
                       const char *name,
                       void (*entry)(void *arg),
                       void *arg,
                       void *stack,
                       int stack_size,
                       unsigned short priority);

/** Delete a task.  Deleting a task is dangerous because if the task
  * is any locks, they will not be returning, which will likely cause
  * a deadlock later on.
  *
  * \param task         Task struct pointer.
  */
void gyros_task_delete(gyros_task_t *task);

/** Wait for any task to finish.  When a task has been returned, all
  * resources for that task can be safely reclaimed.
  *
  * \return             Struct pointer to finished task.  Never @c NULL.
  */
gyros_task_t *gyros_task_wait(void);

/** Wait for any task to finish, or until @a timeout is reached.  When
  *  a task has been returned, all resources for that task can be
  *  safely reclaimed.
  *
  * \param timeout      Timeout.  See gyros_time().
  * \return             Struct pointer to finished task, or @c NULL
  *                     if @a timeout was reached.
  */
gyros_task_t *gyros_task_timedwait(gyros_abstime_t timeout);

/** Return struct pointer to current task.
  *
  * \return             Struct pointer to current task.
  */
gyros_task_t *gyros_task_current(void);

/** Get the priority of @a task.
  *
  * \return             Task priority.
  */
unsigned short gyros_task_get_priority(gyros_task_t *task);

/** Set the priority of @a task to @ priority.
  *
  * \param task         Task struct pointer.
  * \param priority     New priority.
  */
void gyros_task_set_priority(gyros_task_t *task, unsigned short priority);

/** Suspend a task.  The task will be suspended until gyros_task_resume()
  * is called for the task.
  *
  * \param task         Task struct pointer.
  */
void gyros_task_suspend(gyros_task_t *task);

/** Resume a suspended task.
  *
  * \param task         Task struct pointer.
  */
void gyros_task_resume(gyros_task_t *task);

/** Yield the current task.  The current task will be moved last in
  * the list of running tasks of the same priority.
  */
void gyros_yield(void);

/** Return the amount of stack used by @a task.
  *
  * \param task         Task struct pointer.
  * \return             Number of bytes stack used.
  */
int gyros_task_stack_used(gyros_task_t *task);

#endif
