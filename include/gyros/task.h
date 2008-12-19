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

#include <gyros/debug.h>
#include <gyros/list.h>
#include <gyros/target/interrupt.h>
#include <gyros/target/types.h>

typedef struct
{
    struct gyros_task_regs regs;
    struct gyros_list_node main_list;

    struct gyros_list_node timeout_list;
    unsigned long timeout;
    unsigned char timed_out;

    unsigned char raised_priority;
    unsigned short priority;

    struct gyros_list_node task_list;

    const char *name;
    void *stack;
    int stack_size;

#if GYROS_DEBUG
    unsigned debug_magic;
    const char *debug_state;
    void *debug_object;
#endif
} gyros_task_t;

void gyros_start(void) __attribute__((__noreturn__));

void gyros_task_create(gyros_task_t *task,
                       const char *name,
                       void (*entry)(void *arg),
                       void *arg,
                       void *stack,
                       int stack_size,
                       unsigned short priority);

void gyros_task_delete(gyros_task_t *task);

gyros_task_t *gyros_task_wait(void);

gyros_task_t *gyros_task_timedwait(gyros_abstime_t timeout);

gyros_task_t *gyros_task_current(void);

unsigned short gyros_task_get_priority(gyros_task_t *task);

void gyros_task_set_priority(gyros_task_t *task, unsigned short priority);

void gyros_task_suspend(gyros_task_t *task);

void gyros_task_resume(gyros_task_t *task);

void gyros_yield(void);

void gyros_task_lock(void);

void gyros_task_unlock(void);

gyros_task_t *gyros_task_iterate(gyros_task_t *previous);

int gyros_task_stack_used(gyros_task_t *task);

gyros_abstime_t gyros_time(void);

static inline int gyros_time_reached(gyros_abstime_t time)
{
    return (gyros_time_t)(time - gyros_time()) <= 0;
}

int gyros_sleep_until(gyros_abstime_t timeout);

static inline int gyros_sleep_us(int microseconds)
{
    return gyros_sleep_until(gyros_time() + gyros_us(microseconds) + 1);
}

static inline int gyros_sleep_ms(int milliseconds)
{
    return gyros_sleep_until(gyros_time() + gyros_ms(milliseconds) + 1);
}

static inline int gyros_sleep_s(int seconds)
{
    return gyros_sleep_until(gyros_time() + gyros_s(seconds) + 1);
}

#endif
