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
#include <gyros/private/state.h>
#include <gyros/private/target.h>
#include <gyros/timer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TASK(t)     GYROS__LIST_CONTAINER(t, gyros_task_t, main_list_node)
#define TIMEOUT(t)  GYROS__LIST_CONTAINER(t, gyros_task_t, timeout_list_node)
#define TIMER(t)    GYROS__LIST_CONTAINER(t, gyros_timer_t, list_node)

extern gyros_t _gyros;
extern gyros_mutex_t gyros__iterate_mutex;

void gyros__task_finish(gyros_task_t *task);

void gyros__task_suspend(gyros_task_t *task);

void gyros__task_move(gyros_task_t *task, struct gyros__list_node *list);

void gyros__task_wake(gyros_task_t *task);

void gyros__set_priority(gyros_task_t *task, unsigned short priority);

int gyros__task_set_timeout(gyros_abstime_t timeout);

void gyros__timer_schedule(gyros_timer_t *timer);

#if GYROS_CONFIG_DYNTICK
void gyros__dyntick_update(gyros_abstime_t now);
#else
#define gyros__dyntick_update(now) do { } while (0)
#endif

void gyros__cond_reschedule(void);

void gyros__error(const char *msg, void *object);

#ifdef __cplusplus
}
#endif

#endif
