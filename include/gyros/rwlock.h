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
#ifndef INCLUDED__gyros_rwlock_h__200812121950
#define INCLUDED__gyros_rwlock_h__200812121950

#include <gyros/task.h>

typedef struct gyros_rwlock
{
#if GYROS_DEBUG
    unsigned debug_magic;
#endif

    gyros_task_t *writer;
    int readers;

    struct gyros_list_node rd_task_list;
    struct gyros_list_node wr_task_list;
} gyros_rwlock_t;

void gyros_rwlock_init(gyros_rwlock_t *rwlock);

void gyros_rwlock_rdlock(gyros_rwlock_t *rwlock);

int gyros_rwlock_tryrdlock(gyros_rwlock_t *rwlock);

int gyros_rwlock_timedrdlock(gyros_rwlock_t *rwlock, gyros_abstime_t timeout);

void gyros_rwlock_wrlock(gyros_rwlock_t *rwlock);

int gyros_rwlock_trywrlock(gyros_rwlock_t *rwlock);

int gyros_rwlock_timedwrlock(gyros_rwlock_t *rwlock, gyros_abstime_t timeout);

void gyros_rwlock_unlock(gyros_rwlock_t *rwlock);

#endif