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
#ifndef INCLUDED__gyros_private_h__200808271854
#define INCLUDED__gyros_private_h__200808271854

#include <gyros/task.h>
#include <gyros/mutex.h>

#define TASK(t) GYROS_LIST_CONTAINER(t, gyros_task_t, main_list)

typedef struct
{
    int locked;
    gyros_task_t *current;
    struct gyros_list_node running;
} gyros__state_t;

extern struct gyros_list_node gyros__tasks;
extern struct gyros_list_node gyros__zombies;
extern struct gyros_list_node gyros__reapers;
extern gyros__state_t gyros__state;

void gyros__task_zombify(gyros_task_t *task);

void gyros__task_exit(void);

void gyros__task_move(gyros_task_t *task, struct gyros_list_node *list);

void gyros__task_wake(gyros_task_t *task);

void gyros__task_set_timeout(gyros_abstime_t timeout);

void gyros__wake_sleeping_tasks(void);

void gyros__mutex_unlock(gyros_mutex_t *m, int reschedule);

void gyros__cond_reschedule(void);

/* The following functions must be implemented by the target */

void gyros__target_init(void);

void gyros__target_task_init(gyros_task_t *task,
                             void (*entry)(void *arg),
                             void *arg,
                             void *stack,
                             int stack_size);

void gyros__suspend_tick(void);

void gyros__update_tick(gyros_abstime_t next_timeout);

#endif
