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
#include <gyros/target/config.h>

#if GYROS_CONFIG_TRACE

extern int gyros__trace_enabled;

#define GYROS__TRACE_SEM(kind, s)                                             \
    if (unlikely(gyros__trace_enabled))                                       \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_SEM_ ## kind);     \
        gyros__t->info.sem.sem = (s);                                         \
        gyros__t->info.sem.value = (s)->value;                                \
    }

#define GYROS__TRACE_MUTEX(kind, m)                                           \
    if (unlikely(gyros__trace_enabled))                                       \
    {                                                                         \
        gyros_trace_t *gyros__t = gyros__trace(GYROS_TRACE_MUTEX_ ## kind);   \
        gyros__t->info.mutex.mutex = (m);                                     \
    }

#else

#define GYROS__TRACE_SEM(kind, s)
#define GYROS__TRACE_MUTEX(kind, m)

#endif

gyros_trace_t *gyros__trace(enum gyros_trace_kind kind);

#endif
