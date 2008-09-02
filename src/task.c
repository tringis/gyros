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
#include <gyros/task.h>

#include <string.h>

#include <gyros/target/interrupt.h>

#include "private.h"

gyros__state_t gyros__state;
struct gyros_list_node gyros__tasks = { &gyros__tasks, &gyros__tasks };
static gyros_task_t s_idle_task;

static void
add_task_to_list(gyros_task_t *task, struct gyros_list_node *list)
{
    struct gyros_list_node *i;

    for (i = list->next; i != list; i = i->next)
    {
        if (task->priority > TASK(i)->priority)
            break;
    }
    gyros_list_insert_before(&task->main_list, i);
}

void
gyros__task_exit(void)
{
    /* Note that we do not need to call gyros_interrupt_restore()
     * because gyros__reschedule below never returns. */
    gyros_interrupt_disable();
    gyros_list_remove(&gyros__state.current->main_list);
    gyros_list_remove(&gyros__state.current->task_list);
    gyros__reschedule();
}

void
gyros__task_move(gyros_task_t *task, struct gyros_list_node *list)
{
    gyros_list_remove(&task->main_list);
    add_task_to_list(task, list);
}

void
gyros__task_wake(gyros_task_t *task)
{
    gyros_list_remove(&task->main_list);
    gyros_list_remove(&task->timeout_list);

    add_task_to_list(task, &gyros__state.running);
}

void
gyros_init(void)
{
    GYROS_LIST_NODE_INIT(&gyros__state.running);

    gyros__target_init();

    /* Make the current "task" the idle task */
    s_idle_task.priority = 0;

    s_idle_task.name = "[idle]";
    s_idle_task.stack = 0;
    s_idle_task.stack_size = 0;

    gyros_list_insert_before(&s_idle_task.task_list, &gyros__tasks);
    add_task_to_list(&s_idle_task, &gyros__state.running);
    gyros__state.current = &s_idle_task;
}

void
gyros_start(void)
{
    gyros__tick_enable();
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
                  int priority)
{
    unsigned long flags;

    memset(task, 0, sizeof(gyros_task_t));

    task->priority = priority;

    task->name = name;
    task->stack = stack;
    task->stack_size = stack_size;

    /* Perform architecture specific initalization */
    gyros__target_task_init(task, entry, arg, stack, stack_size);

    flags = gyros_interrupt_disable();
    gyros_list_insert_before(&task->task_list, &gyros__tasks);
    add_task_to_list(task, &gyros__state.running);
    GYROS_LIST_NODE_INIT(&task->timeout_list);
    gyros_interrupt_restore(flags);
}

void
gyros_yield(void)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros_list_remove(&gyros__state.current->main_list);
    add_task_to_list(gyros__state.current, &gyros__state.running);
    gyros__cond_reschedule();
    gyros_interrupt_restore(flags);
}
