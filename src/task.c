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
#include <gyros/compiler.h>
#include <gyros/interrupt.h>
#include <gyros/private/trace.h>
#include <gyros/config.h>
#include <gyros/task.h>

#include <limits.h>
#include <stddef.h>

#include "private.h"

static gyros_task_t s_idle_task;

gyros_t gyros = {
    NULL,
    GYROS__LIST_INITVAL(gyros.running),
    GYROS__LIST_INITVAL(gyros.timeouts),
#if GYROS_CONFIG_TIMER
    GYROS__LIST_INITVAL(gyros.timers),
#endif
#if GYROS_CONFIG_ITERATE
    GYROS__LIST_INITVAL(gyros.tasks),
#endif
#if GYROS_CONFIG_WAIT
    GYROS__LIST_INITVAL(gyros.zombies),
    GYROS__LIST_INITVAL(gyros.reapers),
#endif
};

#if GYROS_CONFIG_ITERATE
gyros_mutex_t gyros__iterate_mutex = GYROS_MUTEX_INITVAL(gyros__iterate_mutex);
#endif

static void
add_task_to_list(gyros_task_t *task, struct gyros__list_node *list)
{
    struct gyros__list_node *i;

    for (i = list->next; i != list; i = i->next)
    {
        if (task->priority > TASK(i)->priority)
            break;
    }
    gyros__list_insert_before(&task->main_list_node, i);
    task->main_list = list;
}

void
gyros__task_zombify(gyros_task_t *task)
{
    gyros__task_suspend(gyros.current);
    gyros__list_remove(&gyros.current->timeout_list_node);
#if GYROS_CONFIG_ITERATE
    gyros__list_remove(&gyros.current->task_list_node);
#endif
#if GYROS_CONFIG_WAIT
    gyros__list_insert_before(&task->main_list_node, &gyros.zombies);

    /* Wake the tasks in reverse order to preserve the order of the
     * tasks (of equal priority) in the list. */
    while (gyros.reapers.prev != &gyros.reapers)
        gyros__task_wake(TASK(gyros.reapers.prev));
#endif
}

void
gyros__task_exit(void)
{
    unsigned long flags;

#if GYROS_CONFIG_ITERATE
    gyros_mutex_lock(&gyros__iterate_mutex);
#endif

    flags = gyros_interrupt_disable();
    GYROS_DEBUG_SET_STATE(gyros.current, "zombie");
    gyros__task_zombify(gyros.current);
#if GYROS_CONFIG_ITERATE
    gyros__mutex_unlock_slow(&gyros__iterate_mutex, 0);
#endif
    gyros__reschedule();
    gyros_interrupt_restore(flags);
    gyros__error("task exit with interrupts disabled", gyros.current);
}

void
gyros__task_suspend(gyros_task_t *task)
{
    gyros__list_remove(&task->main_list_node);
    gyros.current->main_list = NULL;
}

void
gyros__task_move(gyros_task_t *task, struct gyros__list_node *list)
{
    gyros__list_remove(&task->main_list_node);
    add_task_to_list(task, list);
}

void
gyros__task_wake(gyros_task_t *task)
{
#if GYROS_CONFIG_TRACE
    if (GYROS_UNLIKELY(gyros__trace_enabled))
        gyros__trace(GYROS_TRACE_WAKE)->info.wake_task = task;
#endif

    GYROS_DEBUG_SET_STATE(task, "running");
    gyros__list_remove(&task->timeout_list_node);
    gyros__task_move(task, &gyros.running);
}

void
gyros__set_priority(gyros_task_t *task, unsigned short priority)
{
    task->priority = priority;
    if (task->main_list)
        gyros__task_move(task, task->main_list);
}

void
gyros__cond_reschedule(void)
{
    if (GYROS_UNLIKELY(TASK(gyros.running.next) != gyros.current))
        gyros__reschedule();
}

void
gyros_start(void)
{
    /* Make the current "task" the idle task */
#if GYROS_CONFIG_DEBUG
    s_idle_task.debug_magic = GYROS_TASK_DEBUG_MAGIC;
#endif
    GYROS_DEBUG_SET_STATE(&s_idle_task, "running");
    s_idle_task.base_priority = 0;
    s_idle_task.priority = 0;
    s_idle_task.name = "[idle]";
    s_idle_task.stack = 0;
    s_idle_task.stack_size = 0;

#if GYROS_CONFIG_ITERATE
    gyros__list_insert_before(&s_idle_task.task_list_node, gyros.tasks.next);
#endif
    add_task_to_list(&s_idle_task, &gyros.running);
    gyros.current = &s_idle_task;
    GYROS__LIST_NODE_INIT(&s_idle_task.timeout_list_node);

    gyros__target_init();

    gyros__reschedule();

#if !GYROS_CONFIG_CUSTOM_IDLE_LOOP
    for (;;)
        gyros__idle();
#endif
}

void
gyros_task_create(gyros_task_t *task,
                  const char *name,
                  void (*entry)(void *arg),
                  void *arg,
                  void *stack,
                  int stack_size,
                  unsigned short priority)
{
    unsigned long flags;

#if GYROS_CONFIG_DEBUG
    task->debug_magic = GYROS_TASK_DEBUG_MAGIC;
#endif
    GYROS_DEBUG_SET_STATE(task, "running");
    task->base_priority = priority;
    task->priority = priority;
    task->name = name;
    task->stack = stack;
    task->stack_size = stack_size;

    /* Perform architecture specific initalization */
    gyros__target_task_init(task, entry, arg, stack, stack_size);

    flags = gyros_interrupt_disable();
#if GYROS_CONFIG_ITERATE
    gyros__list_insert_before(&task->task_list_node, &gyros.tasks);
#endif
    add_task_to_list(task, &gyros.running);
    GYROS__LIST_NODE_INIT(&task->timeout_list_node);
#if GYROS_CONFIG_MESSAGE_QUEUE
    GYROS__LIST_NODE_INIT(&task->msg_list);
    task->receiving = 0;
#endif
    if (gyros.current) /* Don't reschedule before gyros_start() */
        gyros__cond_reschedule();
    gyros_interrupt_restore(flags);
}
