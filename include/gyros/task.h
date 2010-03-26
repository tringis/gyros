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

/** \file
  * \brief Task management.
  * \details Header file for \ref task_group.
  *
  * \defgroup task_group Task management
  *
  * Tasks are described by a task struct called gyros_task_t which is
  * initialized by gyros_task_create().  The task struct is used to
  * store the state of the task when it is not running.  Tasks can be
  * created, suspended, resumed, deleted, and waited for.  Every task
  * has a main function, and if the task returns from this function it
  * will be deleted automatically.  That is the normal way for a task
  * to be deleted.
  *
  * Each task can be either running or suspended, optionally with a
  * timeout that will cause it to become running again after a certain
  * time has been reached.  Each task also has a priority assigned to
  * it, and only the running task with the higher priority is allowed
  * to execute.  If there are more than one task having the highest
  * priority, the one executing will be allowed to execute until it
  * either calls gyros_yield(), or becomes suspended (by blocking on a
  * synchronization object or calls a sleeping function).
  *
  * Also associated with each task is a name used for debugging
  * purposes, and a stack.
  *
  * Here's an example of task creation:
  * \code
  * gyros_task_t task1;
  * char task1_stack[128];
  *
  * static void task1_main(void *arg)
  * {
  *     ...
  * }
  * 
  * void create_task(void)
  * {
  *     gyros_task_create(&task1, "task1", task1_main, 0,
  *                       task1_stack, sizeof(task1_stack), 1);
  * }
  * \endcode
  *
  * @{
  */

#include <gyros/config.h>
#include <gyros/target/context.h>
#include <gyros/private/debug.h>
#include <gyros/private/list.h>
#include <gyros/time.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_TASK_DEBUG_MAGIC                ((unsigned)0xefcd6711)
#endif

/** \brief Task object.
  *
  * This is the structure representing a GyrOS task.  Most members of
  * this stucture are for internal GyrOS use only, and are not meant
  * to be accessed directly. */
typedef struct
{
    struct gyros__context context; /**< \internal */

    struct gyros__list_node main_list_node; /**< \internal */
    struct gyros__list_node *main_list; /**< \internal */

    struct gyros__list_node timeout_list_node; /**< \internal */
    gyros_abstime_t timeout; /**< \internal */
    unsigned char timed_out; /**< \internal */

    unsigned char raised_priority; /**< \internal */
    unsigned short priority; /**< \internal */

#if GYROS_CONFIG_ITERATE
    struct gyros__list_node task_list_node; /**< \internal */
#endif

    const char *name; /**< Task name. */
    void *stack; /**< Task stack base address. */
    int stack_size; /**< Task stack size in bytes. */

#if GYROS_CONFIG_DEBUG
    unsigned debug_magic; /**< \internal */
    const char *debug_state; /**< String representing the task state. */
    /** Pointer to the object related to the current task state,
      * e.g. a pointer to the semaphore debug info if debug_state is
      * "sem_wait". */
    const struct gyros_debug_info *debug_object;
#endif
} gyros_task_t;

/** Start GyrOS.  This call creates the idle task and enables
  * interrupts.  This idle task has priority zero and is always
  * running.  If \ref GYROS_CONFIG_CUSTOM_IDLE_LOOP is disabled, this
  * function will also loop forever as the idle task and never return.
  *
  * If \ref GYROS_CONFIG_CUSTOM_IDLE_LOOP is enabled, this function
  * will return, and the caller is responsible for implementing the
  * idle loop.  Since the idle task must always be running, the custom
  * idle loop must not call functions that may block.
  *
  * The only other GyrOS function that may be called before this
  * function is gyros_task_create().
  */
void gyros_start(void);

/** Create a new task.  If the new task has higher priority than the
  * current task, the current task will be preempted even before
  * returning from this function.
  *
  * This is the only function that may be called before gyros_start().
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

/** Delete a task.  Deleting a task is dangerous because any
  * synchronization objects locked by the deleted objects will remain
  * locked, likely causing deadlocks later on.
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
gyros_task_t *gyros_task_wait_until(gyros_abstime_t timeout);

/** Return struct pointer to current task.  May be called from
  * interrupt context, in which case it returns pointer to the task
  * that was running before the interrupt occured.
  *
  * \return             Struct pointer to current task.
  */
gyros_task_t *gyros_current(void) __attribute__((__const__));

/** Get the priority of @a task.  May be called from interrupt
  * context.
  *
  * \return             Task priority.
  */
unsigned short gyros_task_get_priority(gyros_task_t *task);

/** Set the priority of @a task to @a priority.  May be called from
  * interrupt context.
  *
  * \param task         Task struct pointer.
  * \param priority     New priority.
  */
void gyros_task_set_priority(gyros_task_t *task, unsigned short priority);

/** Suspend a task.  The task will be suspended until gyros_task_resume()
  * is called for the task.  May be called from interrupt context.
  *
  * \param task         Task struct pointer.
  */
void gyros_task_suspend(gyros_task_t *task);

/** Resume a suspended task.  Can also be called for a task waiting
  * for a timeout, in which case it will time out immediately.  May be
  * called from interrupt context.
  *
  * \param task         Task struct pointer.
  */
void gyros_task_resume(gyros_task_t *task);

/** Yield the current task.  The current task will be moved last in
  * the list of running tasks of the same priority, essentially giving
  * other tasks of the same priority a chance to execute.
  */
void gyros_yield(void);

/** Return the amount of stack used by @a task.  Note that \ref
 * GYROS_CONFIG_STACK_USED must be enabled (the default) in order to
 * use this function.
  *
  * \param task         Task struct pointer.
  * \return             Number of bytes stack used.
  */
int gyros_task_stack_used(gyros_task_t *task);

/*@}*/

#endif
