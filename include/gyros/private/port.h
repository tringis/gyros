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
#ifndef INCLUDED__gyros_private_port_h__200901072001
#define INCLUDED__gyros_private_port_h__200901072001

#include <gyros/target/config.h>
#include <gyros/task.h>
#include <gyros/time.h>

/*---------------------------------------------------------------------*'
 * The following function are called by the port, but never from the
 * application.
 *---------------------------------------------------------------------*/

/* Called by the port by the timer interrupt to wake up any sleeping
 * tasks. */
void gyros__wake_timedout_tasks(gyros_abstime_t now);

/* Called by the port when a task exists its main function. */
void gyros__task_exit(void);

/*---------------------------------------------------------------------*
 * The following functions must be implemented by the target *
 *---------------------------------------------------------------------*/

/* Initialize the target by programming a timer to provide the timer
 * interrupt.  Interrupts are disabled when this function is called,
 * and should not be enabled by this function. */
void gyros__target_init(void);

/* Target specific initialization of the task struct. */
void gyros__target_task_init(gyros_task_t *task,
                             void (*entry)(void *arg),
                             void *arg,
                             void *stack,
                             int stack_size);

#if GYROS_CONFIG_DYNTICK
/* Called when there are no timeouts scheduled. */
void gyros__suspend_tick(void);

/* Called to program the timer for the next scheduled timeout. */
void gyros__update_tick(gyros_abstime_t next_timeout);
#endif

#endif