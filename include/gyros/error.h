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
#ifndef INCLUDED__gyros_error_h__201003240827
#define INCLUDED__gyros_error_h__201003240827

#include <gyros/config.h>
#include <gyros/task.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \file
  * \brief Error handling.
  * \details Header file for \ref error_group.
  *
  * \defgroup error_group Error handling
  *
  * When a fatal error is detected by GyrOS, an error handling
  * callback function is called, with a string describing the error as
  * first argument, and an object pointer as second argument.  The
  * meaning of the object pointer depends on the error message, but is
  * normally obvious.  If the error is that there's a mutex deadlock,
  * the pointer points to the mutex causing the deadlock, etc.
  *
  * The callback is implemented by the application, and is typically
  * prints an error message and stops or resets the CPU.  If a
  * callback is not set, GyrOS just disables all interrupts and hangs
  * the CPU.
  *
  * @{
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
void gyros_set_error_handler(void (*handler)(const char *msg, void *object));

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
