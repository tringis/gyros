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
#ifndef INCLUDE__gyros_interrupt_h__200812291812
#define INCLUDE__gyros_interrupt_h__200812291812

/** \file
  * \brief Interrupt management.
  * \details Header file for \ref interrupt_group.
  *
  * \defgroup interrupt_group Interrupt management
  *
  * Interrupts can be disabled and restored to provide the most basic
  * synchronization between a task and an interrupt handler.
  *
  * @{
  */

#include <gyros/target/interrupt.h>

/** Disable all interrupts.
  * \return             Value used to restore interrupts using
  *                     gyros_interrupt_restore().
  */
unsigned long gyros_interrupt_disable(void);

/** Restore interrupts disabled by gyros_interrupt_disable().
  * \param flags        Value returned by gyros_interrupt_restore().
  */
void gyros_interrupt_restore(unsigned long flags);

/** Check whether currently in interrupt context.
  * \return             Non-zero value if called from interrupt context.
  */
int gyros_in_interrupt(void);

/*@}*/

#endif
