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
#ifndef INCLUDED__gyros_trace_h__200905062204
#define INCLUDED__gyros_trace_h__200905062204

#include <gyros/task.h>
#include <gyros/sem.h>
#include <gyros/mutex.h>
#include <gyros/cond.h>
#include <gyros/mq.h>
#include <gyros/rwlock.h>

/** \file trace.h
 * \brief Trace functions.
 */

enum gyros_trace_kind
{
    GYROS_TRACE_EMPTY,
    GYROS_TRACE_TRACE,
    GYROS_TRACE_STRING,
    GYROS_TRACE_RUNNING,
    GYROS_TRACE_IRQ,
    GYROS_TRACE_CONTEXT,
    GYROS_TRACE_WAKE,
    GYROS_TRACE_COND_WAIT,
    GYROS_TRACE_COND_SIGNAL_ONE,
    GYROS_TRACE_COND_SIGNAL_ALL,
    GYROS_TRACE_MQ_SEND,
    GYROS_TRACE_MQ_RECEIVE_BLOCKED,
    GYROS_TRACE_MQ_RECEIVED,
    GYROS_TRACE_MUTEX_BLOCKED,
    GYROS_TRACE_MUTEX_AQUIRED,
    GYROS_TRACE_MUTEX_UNLOCK,
    GYROS_TRACE_RWLOCK_RD_BLOCKED,
    GYROS_TRACE_RWLOCK_RD_AQUIRED,
    GYROS_TRACE_RWLOCK_RD_UNLOCK,
    GYROS_TRACE_RWLOCK_WR_BLOCKED,
    GYROS_TRACE_RWLOCK_WR_AQUIRED,
    GYROS_TRACE_RWLOCK_WR_UNLOCK,
    GYROS_TRACE_SEM_BLOCKED,
    GYROS_TRACE_SEM_AQUIRED,
    GYROS_TRACE_SEM_SIGNAL,
};

struct gyros_trace_context
{
    gyros_task_t *next;
};

struct gyros_trace_wake
{
    gyros_task_t *task;
};

struct gyros_trace_cond
{
    gyros_cond_t *cond;
};

struct gyros_trace_mq
{
    gyros_mq_t *mq;
};

struct gyros_trace_mutex
{
    gyros_mutex_t *mutex;
};

struct gyros_trace_rwlock
{
    gyros_rwlock_t *rwlock;
};

struct gyros_trace_sem
{
    gyros_sem_t *sem;
    unsigned value;
};

typedef struct
{
    enum gyros_trace_kind kind;
    gyros_abstime_t timestamp;
    gyros_task_t *task;

    union
    {
        int trace;
        const char *str;
        const gyros_task_t *running;
        struct gyros_trace_context context;
        struct gyros_trace_wake wake;
        struct gyros_trace_cond cond;
        struct gyros_trace_mq mq;
        struct gyros_trace_mutex mutex;
        struct gyros_trace_rwlock rwlock;
        struct gyros_trace_sem sem;
    } info;
} gyros_trace_t;

void gyros_trace_init(void *log, int log_size);

void gyros_trace_on(void);

void gyros_trace_off(int when);

void gyros_trace_string(const char *str);

void gyros_trace_running_tasks(void);

gyros_trace_t *gyros_trace_iterate(gyros_trace_t *prev);

#endif
