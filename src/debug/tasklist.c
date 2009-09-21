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
#include <gyros/iterate.h>

#if GYROS_CONFIG_ITERATE

/* Local implementation to avoid string.h dependency. */
static int
streq(const char *s1, const char *s2)
{
    for (;;)
    {
        if (*s1 != *s2)
            return 0;
        if (*s1 == 0)
            return 1;
        ++s1;
        ++s2;
    }
}

void
gyros_debug_task_list(void (*pf)(void *arg, char *fmt, ...), void *arg)
{
    gyros_task_t *t;
    int i;

    pf(arg, "%s", "name              prio  stack  task       "
#if GYROS_CONFIG_DEBUG
       "state"
#else
       "pc"
#endif
       "\n");
    for (i = 0; i < 79; ++i)
        pf(arg, "-");
    pf(arg, "\n");

    for (t = gyros_task_iterate(0); t; t = gyros_task_iterate(t))
    {
        pf(arg, "%-16s %3d %4d/%4d %p ",
           t->name, t->priority,
           gyros_task_stack_used(t), t->stack_size, t);
#if GYROS_CONFIG_DEBUG
        pf(arg, "%s", t->debug_state);
        if (streq(t->debug_state, "running"))
            pf(arg, " @ 0x%08x", t->context.pc);
        else if (t->debug_object)
        {
            if (t->debug_object->name)
                pf(arg, " @ %s", t->debug_object->name);
            else
                pf(arg, " @ %p", t->debug_object);
        }
#else
        pf(arg, "0x%08x", t->context.pc);
#endif
        pf(arg, "\n");
    }
}

#endif