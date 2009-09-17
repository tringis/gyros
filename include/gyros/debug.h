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

#include <gyros/target/config.h>

/** \defgroup debug_group Debugging support
  *
  * Debugging is enabled when compiled with @c GYROS_CONFIG_DEBUG set
  * to a non-zero value.  When debugging is enabled, many common
  * invalid uses of the GyrOS API is detected, and more information
  * about the status of each task tracked, e.g. if a task is blocking
  * on a mutex, and if so, which mutex.
  */
/*@{*/

/** \file
  * \brief Debugging support.
  * \copydoc debug_group
  */

/** Set the error handler to @a msg. The default error handler just
  * hangs forever in a loop with interrupts disabled.  The error
  * handler is called with two parameters, both a text message
  * describing the error, and a pointer to the object relevant to the
  * error, e.g. a pointer to the mutex when a mutex error is detected,
  * or a task when a task error is detected, or @c NULL when there is
  * no relevant object.
  *
  * \param handler          Error handler function pointer.
  */
#if GYROS_CONFIG_DEBUG
void gyros_set_error_handler(void (*handler)(const char *msg,
                                             void *object));
#else
#define gyros_set_error_handler(handler) do { } while (0)
#endif

/** Set the name of the @a object to @a object_name.
  *
  * \param object           Object to name.
  * \param object_name      New name of object.  Note that only the pointer
  *                         to the name is stored, so the pointer must remain
  *                         valid for the lifetime of the object.
  */
#if GYROS_CONFIG_DEBUG
#define gyros_debug_set_name(object, object_name)   \
    ((object)->debug_info.name = (name))
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

/*@}*/

#endif
