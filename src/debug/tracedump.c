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
#include <gyros/debug.h>
#include <gyros/trace.h>

#include <stddef.h>

#if GYROS_CONFIG_TRACE

#if GYROS_CONFIG_DEBUG
#define DUMP_OBJECT(event, obj)                                         \
    if ((obj)->debug_info.name)                                         \
        pf(arg, "%-15s %s", (event), (obj)->debug_info.name);            \
    else                                                                \
        pf(arg, "%-15s %p", (event), (obj));
#else
#define DUMP_OBJECT(event, obj) pf(arg, "%-15s %p", (event), (obj));
#endif

static const char*
trace_kind_type(enum gyros_trace_kind kind)
{
    switch (kind)
    {
    case GYROS_TRACE_EMPTY:
        return "";
    case GYROS_TRACE_TRACE:
        return "*TRACE*";
    case GYROS_TRACE_STRING:
        return "STRING";
    case GYROS_TRACE_RUNNING:
        return "RUNNING";
    case GYROS_TRACE_IRQ:
        return "IRQ";
    case GYROS_TRACE_CONTEXT:
        return "CONTEXT";
    case GYROS_TRACE_WAKE:
        return "WAKE";
    case GYROS_TRACE_COND_WAIT:
    case GYROS_TRACE_COND_SIGNAL_ONE:
    case GYROS_TRACE_COND_SIGNAL_ALL:
        return "COND";
    case GYROS_TRACE_TIMER_SET:
    case GYROS_TRACE_TIMER_SET_PERIODIC:
    case GYROS_TRACE_TIMER_CLEAR:
    case GYROS_TRACE_TIMER_CALLBACK:
        return "TIMER";
    case GYROS_TRACE_MQ_SEND:
    case GYROS_TRACE_MQ_RECEIVE_BLOCKED:
    case GYROS_TRACE_MQ_RECEIVED:
        return "MQ";
    case GYROS_TRACE_MUTEX_BLOCKED:
    case GYROS_TRACE_MUTEX_AQUIRED:
    case GYROS_TRACE_MUTEX_UNLOCK:
        return "MUTEX";
    case GYROS_TRACE_RWLOCK_RD_BLOCKED:
    case GYROS_TRACE_RWLOCK_RD_AQUIRED:
    case GYROS_TRACE_RWLOCK_RD_UNLOCK:
    case GYROS_TRACE_RWLOCK_WR_BLOCKED:
    case GYROS_TRACE_RWLOCK_WR_AQUIRED:
    case GYROS_TRACE_RWLOCK_WR_UNLOCK:
        return "RWLOCK";
    case GYROS_TRACE_SEM_BLOCKED:
    case GYROS_TRACE_SEM_AQUIRED:
    case GYROS_TRACE_SEM_SIGNAL:
        return "SEM";
    }

    return "UNKNOWN";
}

void
gyros_debug_trace_dump(void (*pf)(void *arg, char *fmt, ...), void *arg)
{
    gyros_trace_t *t, *next_t;
    int i = 0;

    gyros_trace_off(0);
    t = gyros_trace_iterate(NULL);
    next_t = gyros_trace_iterate(t);
    while (t)
    {
        if (t->kind == GYROS_TRACE_EMPTY)
        {
            t = next_t;
            next_t = gyros_trace_iterate(t);
            continue;
        }

        pf(arg, "%3u %6llu.%06llu [%-8s] %-8s%s",
           i++, t->timestamp / 1000000,
           t->timestamp - t->timestamp / 1000000 * 1000000,
           t->task ? t->task->name : "",
           trace_kind_type(t->kind),
           t->kind == GYROS_TRACE_IRQ ? "" : " ");

        switch (t->kind)
        {
        case GYROS_TRACE_EMPTY:
            break;
        case GYROS_TRACE_TRACE:
            if (t->info.trace < 0)
                pf(arg, "%d", t->info.trace);
            else
                pf(arg, t->info.trace ? "on" : "off");
            break;
        case GYROS_TRACE_STRING:
            pf(arg, "%s", t->info.str);
            break;
        case GYROS_TRACE_RUNNING:
            pf(arg, "%s", t->info.running->name);
            break;
        case GYROS_TRACE_IRQ:
            break;
        case GYROS_TRACE_CONTEXT:
            pf(arg, "%s => %s", t->task->name, t->info.context_next->name);
            break;
        case GYROS_TRACE_WAKE:
            pf(arg, "%s", t->info.wake_task->name);
            break;

#ifdef GYROS_CONFIG_TIMER
        case GYROS_TRACE_TIMER_SET:
            DUMP_OBJECT("SET", t->info.timer_set.timer);
            pf(arg, ", timeout=%llu.%06llu",
               t->info.timer_set.timeout / 1000000,
               t->info.timer_set.timeout -
               t->info.timer_set.timeout / 1000000 * 1000000);
            break;
        case GYROS_TRACE_TIMER_SET_PERIODIC:
            DUMP_OBJECT("SET_PERIODIC", t->info.timer_set_periodic.timer);
            pf(arg, ", period=%llu.%06llu",
               t->info.timer_set_periodic.period / 1000000,
               t->info.timer_set_periodic.period -
               t->info.timer_set_periodic.period / 1000000 * 1000000);
            break;
        case GYROS_TRACE_TIMER_CLEAR:
            DUMP_OBJECT("CLEAR", t->info.timer);
            break;
        case GYROS_TRACE_TIMER_CALLBACK:
            DUMP_OBJECT("CALLBACK", t->info.timer);
            break;
#endif

        case GYROS_TRACE_COND_WAIT:
            DUMP_OBJECT("WAIT", t->info.cond);
            break;
        case GYROS_TRACE_COND_SIGNAL_ONE:
            DUMP_OBJECT("SIGNAL_ONE", t->info.cond);
            break;
        case GYROS_TRACE_COND_SIGNAL_ALL:
            DUMP_OBJECT("SIGNAL_ALL", t->info.cond);
            break;

        case GYROS_TRACE_MQ_SEND:
            DUMP_OBJECT("SEND", t->info.mq);
            break;
        case GYROS_TRACE_MQ_RECEIVE_BLOCKED:
            DUMP_OBJECT("RECEIVE_BLOCKED", t->info.mq);
            break;
        case GYROS_TRACE_MQ_RECEIVED:
            DUMP_OBJECT("RECEIVED", t->info.mq);
            break;

        case GYROS_TRACE_MUTEX_BLOCKED:
            DUMP_OBJECT("BLOCKED", t->info.mutex);
            break;
        case GYROS_TRACE_MUTEX_AQUIRED:
            DUMP_OBJECT("AQUIRED", t->info.mutex);
            break;
        case GYROS_TRACE_MUTEX_UNLOCK:
            DUMP_OBJECT("UNLOCK", t->info.mutex);
            break;

        case GYROS_TRACE_RWLOCK_RD_BLOCKED:
            DUMP_OBJECT("RD_BLOCKED", t->info.rwlock);
            break;
        case GYROS_TRACE_RWLOCK_RD_AQUIRED:
            DUMP_OBJECT("RD_AQUIRED", t->info.rwlock);
            break;
        case GYROS_TRACE_RWLOCK_RD_UNLOCK:
            DUMP_OBJECT("RD_UNLOCK", t->info.rwlock);
            break;
        case GYROS_TRACE_RWLOCK_WR_BLOCKED:
            DUMP_OBJECT("WR_BLOCKED", t->info.rwlock);
            break;
        case GYROS_TRACE_RWLOCK_WR_AQUIRED:
            DUMP_OBJECT("WR_AQUIRED", t->info.rwlock);
            break;
        case GYROS_TRACE_RWLOCK_WR_UNLOCK:
            DUMP_OBJECT("WR_UNLOCK", t->info.rwlock);
            break;

        case GYROS_TRACE_SEM_BLOCKED:
            DUMP_OBJECT("BLOCKED", t->info.sem.sem);
            pf(arg, " (%u)", t->info.sem.value);
            break;
        case GYROS_TRACE_SEM_AQUIRED:
            DUMP_OBJECT("AQUIRED", t->info.sem.sem);
            pf(arg, " (%u)", t->info.sem.value);
            break;
        case GYROS_TRACE_SEM_SIGNAL:
            DUMP_OBJECT("SIGNAL", t->info.sem.sem);
            pf(arg, " (%u)", t->info.sem.value);
            break;
        }
        if (t->kind != GYROS_TRACE_EMPTY)
        {
            if (next_t)
                pf(arg, " [%llu]", t->timestamp - next_t->timestamp);
            pf(arg, "\n");
        }

        t = next_t;
        next_t = gyros_trace_iterate(t);
    }
}

#endif