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
#ifndef INCLUDED__gyros_hooks_h__200905061636
#define INCLUDED__gyros_hooks_h__200905061636

#include <gyros/task.h>

/** \file
  * \brief Hook support.
  * \details Header file for \ref hook_group.
  *
  * \defgroup hook_group Hook support
  *
  * Hooks are used to call custom code for certain internal gyros
  * events such as interrupts and context switches.  While the hooks
  * are designed to be efficient, there is a performance penalty,
  * which is why hook support is disabled by default.
  *
  * GyrOS must be compiled with @c GYROS_CONFIG_CONTEXT_HOOK set to a
  * non-zero value to enable support for the context switch hook, and
  * @c GYROS_CONFIG_IRQ_HOOK must be set to a non-zero value to enable
  * the IRQ hook.
  *
  * @{
  */

/** Set the context switch hook function to @a context_hook, which can
  * be @c NULL to disable the hook.  The hook function is called with
  * pointers to the current and next task structs.
  *
  * \param context_hook     Context switch hook function pointer.
  */
void gyros_set_context_hook(void (*context_hook)(gyros_task_t *current,
                                                 gyros_task_t *next));

/** Set the IRQ switch hook function to @a irq_hook, which can be @c
  * NULL to disable the hook.
  *
  * \param irq_hook         IRQ hook function pointer.
  */
void gyros_set_irq_hook(void (*irq_hook)(void));

/*@}*/

#endif
