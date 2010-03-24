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
#ifndef INCLUDED__gyros_private_h__200808271854
#define INCLUDED__gyros_private_h__200808271854

/*---------------------------------------------------------------------*
 * This file is completely internal to GyrOS and contains stuff that
 * are not used by neither the application nor the target port.
 *---------------------------------------------------------------------*/

#include <gyros/task.h>
#include <gyros/mutex.h>
#include <gyros/private/target.h>
#include <gyros/timer.h>

#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#define TASK(t)     GYROS__LIST_CONTAINER(t, gyros_task_t, main_list_node)
#define TIMEOUT(t)  GYROS__LIST_CONTAINER(t, gyros_task_t, timeout_list_node)
#define TIMER(t)    GYROS__LIST_CONTAINER(t, gyros_timer_t, list_node)

typedef struct
{
    gyros_task_t *current;
    struct gyros__list_node running;
    struct gyros__list_node timeouts;

#if GYROS_CONFIG_TIMER
    struct gyros__list_node timers;
#endif

#if GYROS_CONFIG_ITERATE
    struct gyros__list_node tasks;
    gyros_mutex_t iterate_mutex;
#endif

#if GYROS_CONFIG_WAIT
    struct gyros__list_node zombies;
    struct gyros__list_node reapers;
#endif
} gyros_t;

extern gyros_t gyros;

void gyros__task_zombify(gyros_task_t *task);

void gyros__task_move(gyros_task_t *task, struct gyros__list_node *list);

void gyros__task_wake(gyros_task_t *task);

void gyros__task_set_timeout(gyros_abstime_t timeout);

void gyros__timer_schedule(gyros_timer_t *timer);

#if GYROS_CONFIG_DYNTICK
void gyros__dyntick_update(gyros_abstime_t now);
#else
#define gyros__dyntick_update(now) do { } while (0)
#endif

void gyros__mutex_unlock(gyros_mutex_t *m, int reschedule);

void gyros__cond_reschedule(void);

void gyros__error(const char *msg, void *object);

#endif
