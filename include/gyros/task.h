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
#ifndef INCLUDED__gyros_task_h__200206071335
#define INCLUDED__gyros_task_h__200206071335

#include <gyros/list.h>
#include <gyros/target/interrupt.h>
#include <gyros/target/task_regs.h>

typedef struct
{
    struct gyros_task_regs regs;
    struct gyros_list_node main_list;

    struct gyros_list_node timeout_list;
    unsigned long timeout;
    short timed_out;

    struct gyros_list_node task_list;

    int priority;

    const char *name;
    void *stack;
    int stack_size;
} gyros_task_t;

void gyros_init(void);

void gyros_start(void) __attribute__((__noreturn__));

void gyros_task_create(gyros_task_t *task,
                       const char *name,
                       void (*entry)(void *arg),
                       void *arg,
                       void *stack,
                       int stack_size,
                       int priority);

void gyros_task_delete(gyros_task_t *task);

gyros_task_t *gyros_task_wait(gyros_task_t *task);

gyros_task_t *gyros_task_timedwait(gyros_task_t *task, int timeout);

gyros_task_t *gyros_task_current(void);

void gyros_task_suspend(gyros_task_t *task);

void gyros_task_resume(gyros_task_t *task);

void gyros_yield(void);

void gyros_lock(void);

void gyros_unlock(void);

gyros_task_t *gyros_task_iterate(gyros_task_t *previous);

int gyros_task_stack_used(gyros_task_t *task);

static __inline__ unsigned long
gyros_time(void)
{
    extern unsigned long gyros__ticks;

    return gyros__ticks;
}

int gyros_sleep(unsigned long ticks);

int gyros_sleep_until(unsigned long ticks);

#endif
