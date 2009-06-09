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

#include <gyros/target/config.h>

/**************************************************************************
 * Macros                                                                 *
 **************************************************************************/

/*------------------------------------------------------------------------*
 | save_context - stores current task state in task struct                |
 |              - must not be run in user or system mode                  |
 *------------------------------------------------------------------------*/
.macro save_context
	/* Push lr to the SVC mode stack */
	stmdb	sp!, {lr}
	/* Load address of current task_t to lr */
	ldr	lr, =gyros
	ldr	lr, [lr]
	/* Push SYS mode r0-lr to current task_t */
	stmia	lr, {r0-lr}^
	nop
	mrs	r0, spsr
	str	r0, [lr, #16*4]
	/* Pop lr from the SVC mode stack and write it to the current task_t */
	ldmia	sp!, {r0}
	str	r0, [lr, #15*4]
.endm

/*------------------------------------------------------------------------*
 | restore_context - restores state of next task from its task struct     |
 |                 - must not be run in user or system mode               |
 *------------------------------------------------------------------------*/
.macro restore_context
#if GYROS_CONFIG_CONTEXT_HOOK || GYROS_CONFIG_TRACE
	ldr	r0, =gyros__context_hook
	mov	lr, pc
	bx	r0
#endif
	/* Load address of gyros to r0 */
	ldr	r0, =gyros
	/* Load gyros.running.next to lr */
	ldr	r1, [r0, #4]
	/* Set gyros.current */
	sub	lr, r1, #17*4
	str	lr, [r0]
	/* Get the SPSR from the task struct. */ 
	ldr	r0, [lr, #16*4]
	msr	spsr_cxsf, r0
	/* Restore all system mode registers for the task. */ 
	ldmia	lr, {r0-r14}^
	nop
	/* Restore the return address. */ 
	ldr	lr, [lr, #15*4]
	/* And return - correcting the offset in the LR to obtain the */ 
	/* correct address. */ 
	subs	pc, lr, #4
.endm
