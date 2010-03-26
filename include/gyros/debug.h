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
#ifndef INCLUDED__gyros_debug_h__200812221657
#define INCLUDED__gyros_debug_h__200812221657

#include <gyros/config.h>
#include <gyros/task.h>

/** \file
  * \brief Debugging support.
  * \details Header file for \ref debug_group.
  *
  * \defgroup debug_group Debugging support
  *
  * Debugging is enabled by setting \ref GYROS_CONFIG_DEBUG to a
  * non-zero value in gyros/config.h.  When debugging is
  * enabled, many common invalid uses of the GyrOS API is detected,
  * and more information about the status of each task tracked,
  * e.g. if a task is blocking on a mutex, and if so, which mutex.
  * Errors are reported using the standard
  * \ref error_group "error handling" module.
  *
  * @{
  */

/** Set the name of the @a object to @a object_name.
  *
  * \param object           Object to name.
  * \param object_name      New name of object.  Note that only the pointer
  *                         to the name is stored, so the pointer must remain
  *                         valid for the lifetime of the object.
  */
#if GYROS_CONFIG_DEBUG
#define gyros_debug_set_name(object, object_name)   \
    ((object)->debug_info.name = (object_name))
#else
#define gyros_debug_set_name(object, object_name)   do { } while (0)
#endif

/** Get the name of the @a object to @a object_name, or @c NULL if the
  * object is unnamed.
  *
  * \param object           Object.
  */
#if GYROS_CONFIG_DEBUG
#define gyros_debug_get_name(object)   ((object)->debug_info.name)
#else
#define gyros_debug_get_name(object)   ((const char*)0)
#endif

/** Print all tasks and related debug information using the supplied
  * printf function @a printf_func.  The printf function is called
  * with @a printf_arg as first argument.
  *
  * This function requires GYROS_CONFIG_ITERATE to be enabled, and
  * displays more details if GYROS_CONFIG_DEBUG is enabled, but does
  * work without it.
  *
  * \param printf_func      Pointer to printf function.
  * \param printf_arg       First argument to printf function.
  */
void gyros_debug_task_list(void (*printf_func)(void *arg, char *fmt, ...),
                           void *printf_arg);

/** Print all recorded traces using the supplied printf function @a
  * printf_func.  The printf function is called with @a printf_arg as
  * first argument.
  *
  * This function requires GYROS_CONFIG_TRACE to be enabled, and
  * displays more details if GYROS_CONFIG_DEBUG is enabled, but does
  * work without it.
  *
  * \param printf_func      Pointer to printf function.
  * \param printf_arg       First argument to printf function.
  */
void gyros_debug_trace_dump(void (*printf_func)(void *arg, char *fmt, ...),
                            void *printf_arg);

/** Get the current program counter (PC) for @a task.
  *
  * \param task             Task struct pointer.
  * \return                 Task program counter (PC).
  */
unsigned long gyros_debug_task_pc(gyros_task_t *task);

/*@}*/

#endif
