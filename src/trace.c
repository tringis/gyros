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
#include <gyros/trace.h>

#include <stddef.h>

#include "private.h"

int gyros__trace_enabled;

static gyros_trace_t *s_log_begin;
static gyros_trace_t *s_log_end;
static gyros_trace_t *s_log_pos;

void
gyros_trace_init(void *log, int log_size)
{
    int n = log_size / sizeof(gyros_trace_t);

    s_log_begin = log; /* TODO: Fix alignment!!! */
    s_log_end = s_log_begin + n;
    s_log_pos = s_log_begin;
}

void
gyros_trace_enable(void)
{
    gyros__trace_enabled = 1;
}

void
gyros_trace_disable(void)
{
    gyros__trace_enabled = 0;
}

gyros_trace_t*
gyros_trace_iterate(gyros_trace_t *prev)
{
    if (!prev)
        return s_log_pos;

    return ++prev == s_log_pos ? NULL : prev;
}

gyros_trace_t*
gyros__trace(enum gyros_trace_kind kind)
{
    gyros_trace_t *t = s_log_pos++;

    if (unlikely(s_log_pos == s_log_end))
        s_log_pos = s_log_begin;

    t->timestamp = gyros_time();
    t->task = gyros__state.current;

    return t;
}
