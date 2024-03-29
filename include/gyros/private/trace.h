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
#ifndef INCLUDED__gyros_private_trace_h__200905062242
#define INCLUDED__gyros_private_trace_h__200905062242

#include <gyros/trace.h>
#include <gyros/compiler.h>
#include <gyros-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if GYROS_CONFIG_TRACE

extern int gyros__trace_enabled;

#define GYROS__TRACE_TIMER(kind, t)                                           \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_TIMER_ ## kind);   \
        gyros__t->info.timer = (t);                                           \
    }

#define GYROS__TRACE_TIMER_START(t)                                           \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_TIMER_START);      \
        gyros__t->info.timer_start.timer = (t);                               \
        gyros__t->info.timer_start.timeout = (t)->timeout;                    \
        gyros__t->info.timer_start.period = (t)->period;                      \
    }

#define GYROS__TRACE_COND(kind, c)                                            \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_COND_ ## kind);    \
        gyros__t->info.cond = (c);                                            \
    }

#define GYROS__TRACE_MQ(kind, mq)                                             \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_MQ_ ## kind);      \
        gyros__t->info.mq = (mq);                                             \
    }

#define GYROS__TRACE_SMQ(kind, smq)                                           \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_SMQ_ ## kind);     \
        gyros__t->info.smq = (smq);                                           \
    }

#define GYROS__TRACE_MUTEX(kind, m)                                           \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_MUTEX_ ## kind);   \
        gyros__t->info.mutex = (m);                                           \
    }

#define GYROS__TRACE_RWLOCK(kind, rwlock)                                     \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_RWLOCK_ ## kind);  \
        gyros__t->info.rwlock = (rwlock);                                     \
    }

#define GYROS__TRACE_SEM(kind, s)                                             \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_SEM_ ## kind);     \
        gyros__t->info.sem.sem = (s);                                         \
        gyros__t->info.sem.value = (s)->value;                                \
    }

#define GYROS__TRACE_EVENT_GROUP(kind, e)                                     \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t =                                             \
            gyros__trace(GYROS_TRACE_EVENT_GROUP_ ## kind);                   \
        gyros__t->info.event_group.event_group = (e);                         \
        gyros__t->info.event_group.events = (e)->events;                      \
    }

#define GYROS__TRACE_ALARM_CLOCK(kind, a)                                     \
    if (GYROS_UNLIKELY(gyros__trace_enabled))                                 \
    {                                                                         \
        gyros_trace_t *gyros__t =                                             \
            gyros__trace(GYROS_TRACE_ALARM_CLOCK_ ## kind);                   \
        gyros__t->info.alarm_clock.alarm_clock = (a);                         \
        gyros__t->info.alarm_clock.wakeup_time = (a)->wakeup_time;            \
    }

#else

#define GYROS__TRACE_TIMER(kind, t)
#define GYROS__TRACE_TIMER_START(t)
#define GYROS__TRACE_COND(kind, c)
#define GYROS__TRACE_MQ(kind, mq)
#define GYROS__TRACE_SMQ(kind, smq)
#define GYROS__TRACE_MUTEX(kind, m)
#define GYROS__TRACE_RWLOCK(kind, rwlock)
#define GYROS__TRACE_SEM(kind, s)
#define GYROS__TRACE_EVENT_GROUP(kind, s)
#define GYROS__TRACE_ALARM_CLOCK(kind, a)

#endif

gyros_trace_t *gyros__trace(enum gyros_trace_kind kind);

void gyros__trace_context(gyros_task_t *next);

#ifdef __cplusplus
}
#endif

#endif
