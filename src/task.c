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
#include <gyros/interrupt.h>
#include <gyros/target/config.h>
#include <gyros/task.h>

#include <limits.h>
#include <stddef.h>

#include "private.h"

struct gyros__list_node gyros__tasks = GYROS__LIST_INITVAL(gyros__tasks);
#if GYROS_CONFIG_WAIT
struct gyros__list_node gyros__zombies = GYROS__LIST_INITVAL(gyros__zombies);
struct gyros__list_node gyros__reapers = GYROS__LIST_INITVAL(gyros__reapers);
#endif
static gyros_task_t s_idle_task;
gyros__state_t gyros__state = {
    &s_idle_task,
    GYROS__LIST_INITVAL(gyros__state.running)
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
    gyros__list_insert_before(&task->main_list, i);
}

void
gyros__task_zombify(gyros_task_t *task)
{
    gyros__list_remove(&gyros__state.current->main_list);
    gyros__list_remove(&gyros__state.current->timeout_list);
    gyros__list_remove(&gyros__state.current->task_list);
#if GYROS_CONFIG_WAIT
    gyros__list_insert_before(&task->task_list, &gyros__zombies);

    /* Wake the tasks in reverse order to preserve the order of the
     * tasks (of equal priority) in the list. */
    while (gyros__reapers.prev != &gyros__reapers)
        gyros__task_wake(TASK(gyros__reapers.prev));
#endif
}

void
gyros__task_exit(void)
{
#if GYROS_CONFIG_ITERATE
    gyros_mutex_lock(&gyros__iterate_mutex);
#endif

    /* Note that we do not need to call gyros_interrupt_restore()
     * because gyros__reschedule below never returns. */
    gyros_interrupt_disable();
#if GYROS_CONFIG_DEBUG
    gyros__state.current->debug_state = "zombie";
    gyros__state.current->debug_object = NULL;
#endif
    gyros__task_zombify(gyros__state.current);
#if GYROS_CONFIG_ITERATE
    gyros__mutex_unlock(&gyros__iterate_mutex, 0);
#endif
    gyros__reschedule();
}

void
gyros__task_move(gyros_task_t *task, struct gyros__list_node *list)
{
    gyros__list_remove(&task->main_list);
    add_task_to_list(task, list);
}

void
gyros__task_wake(gyros_task_t *task)
{
#if GYROS_CONFIG_DEBUG
    task->debug_state = "running";
    task->debug_object = NULL;
#endif
    gyros__list_remove(&task->timeout_list);
    gyros__task_move(task, &gyros__state.running);
}

void
gyros__cond_reschedule(void)
{
    unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
    if (gyros_in_interrupt())
        gyros_error("cond_reschedule called from interrupt");
#endif

    if (TASK(gyros__state.running.next) != gyros__state.current)
        gyros__reschedule();
    gyros_interrupt_restore(flags);
}

void
gyros_start(void)
{
    /* Make the current "task" the idle task */
#if GYROS_CONFIG_DEBUG
    s_idle_task.debug_magic = GYROS_TASK_DEBUG_MAGIC;
    s_idle_task.debug_state = "running";
    s_idle_task.debug_object = NULL;
#endif
    s_idle_task.priority = 0;
    s_idle_task.name = "[idle]";
    s_idle_task.stack = 0;
    s_idle_task.stack_size = 0;

    gyros__list_insert_before(&s_idle_task.task_list, gyros__tasks.next);
    add_task_to_list(&s_idle_task, &gyros__state.running);
    gyros__state.current = &s_idle_task;

    gyros__target_init();
    gyros__interrupt_enable();

    for (;;)
        ;
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
    task->debug_state = "running";
    task->debug_object = NULL;
#endif
    task->priority = priority;
    task->name = name;
    task->stack = stack;
    task->stack_size = stack_size;

    /* Perform architecture specific initalization */
    gyros__target_task_init(task, entry, arg, stack, stack_size);

    flags = gyros_interrupt_disable();
    gyros__list_insert_before(&task->task_list, &gyros__tasks);
    add_task_to_list(task, &gyros__state.running);
    GYROS__LIST_NODE_INIT(&task->timeout_list);
    gyros_interrupt_restore(flags);
}
