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
#ifndef INCLUDED__gyros_iterate_h__200812301809
#define INCLUDED__gyros_iterate_h__200812301809

/** \file
  * \brief Task iteration.
  * \details Header file for \ref iteration_group.
  *
  * \defgroup iteration_group Task iteration
  *
  * Task iteration is used to iterate through all tasks.  This is
  * mostly useful for debug and error handling purposes.
  *
  * @{
  */

#include <gyros/task.h>

/** Iterate through all tasks.  Start iteration by calling this
  * function with @a previous set to @c NULL.  Continue calling this
  * function with the value returned by the previous call until @c
  * NULL is returned.  This function locks task creation and deletion
  * during the iteration (causing calls to gyros_task_create and
  * gyros_task_delete to block), and must therefore always be called
  * until it returns @c NULL.
  *
  * \param previous     Task struct pointer.
  * \return             Task struct pointer of next task, or @c NULL
  *                     when iteration is complete.
  */
gyros_task_t *gyros_task_iterate(gyros_task_t *previous);

/*@}*/

#endif
