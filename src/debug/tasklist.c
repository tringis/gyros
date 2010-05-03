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
#include <string.h>

#include <gyros/debug.h>
#include <gyros/iterate.h>

#if GYROS_CONFIG_ITERATE

void
gyros_debug_task_list(void (*printf_func)(void *arg, char *fmt, ...),
                      void *printf_arg)
{
    gyros_task_t *t;
    int i;

    printf_func(printf_arg, "%-16s %7s %-9s %-10s %s\n",
                "name", "prio", "  stack", "task",
#if GYROS_CONFIG_DEBUG
                "state"
#else
                "pc"
#endif
                );
    for (i = 0; i < 79; ++i)
        printf_func(printf_arg, "-");
    printf_func(printf_arg, "\n");
    
    for (t = gyros_task_iterate(0); t; t = gyros_task_iterate(t))
    {
        printf_func(printf_arg, "%-16s ", t->name);
        if (t->priority == t->base_priority)
            printf_func(printf_arg, "%7d ", t->priority);
        else
        {
            printf_func(printf_arg, "%3d+%3d",
                        t->base_priority, t->priority - t->base_priority);
        }
        printf_func(printf_arg, "%4d/%4d %p ",
                    gyros_task_stack_used(t), t->stack_size, t);
#if GYROS_CONFIG_DEBUG
        printf_func(printf_arg, "%s", t->debug_state);
        if (strcmp(t->debug_state, "running") == 0)
            printf_func(printf_arg, " @ 0x%08x", gyros_debug_task_pc(t));
        else if (t->debug_object)
        {
            if (t->debug_object->name)
                printf_func(printf_arg, " @ %s", t->debug_object->name);
            else
                printf_func(printf_arg, " @ %p", t->debug_object);
        }
#else
        printf_func(printf_arg, "0x%08x", gyros_debug_task_pc(t));
#endif
        if (!gyros__list_empty(&t->timeout_list_node))
        {
            printf_func(printf_arg, " (%ld ms)",
                        gyros_time_to_ms(t->timeout - gyros_time()));
        }
        printf_func(printf_arg, "\n");
    }
}

#endif
