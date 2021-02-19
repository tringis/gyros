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
#include <gyros-config.h>

	.syntax unified
	.text

	.align	2
	.thumb_func
	.global	PendSV_Handler
	.type	PendSV_Handler, %function
PendSV_Handler:
#if GYROS_CONFIG_CONTEXT_HOOK || GYROS_CONFIG_TRACE
	push	{r1,lr}		/* Push r1 for ABI's 8 byte stack alignment */
	ldr	r0, =gyros__context_hook
	blx	r0
	pop	{r1,lr}
#endif
	mrs	r3, psp
	stmdb	r3, {r4-r11}	     /* Write registers to task stack. */
	ldr	r0, =gyros	     /* r0 = &gyros.current */
	ldr	r1, [r0]	     /* r1 = gyros.current */
	str	r3, [r1]	     /* gyros.current->context.sp = PSP */

	cpsid	i		     /* disable exceptions */

	ldr	r2, [r0, #4]	     /* r2 = gyros.running.next */
	subs	r2, r2, #4	     /* r2 = TASK(gyros.running.next) */
	str	r2, [r0]	     /* gyros.current = r2 */

	cpsie	i		     /* reenable exceptions */

	ldr	r3, [r2]	     /* r3 = SP of new task */
	ldmdb	r3, {r4-r11}	     /* Load regs for new task */
	msr	psp, r3		     /* Set PSP to SP of new task */
	bx	lr

	.align	2
	.thumb_func
	.global	gyros__arch_enable_thread_stack
	.type	gyros__arch_enable_thread_stack, %function
gyros__arch_enable_thread_stack:
	mov	r1, sp
	msr	psp, r1		     /* PSP = SP */
	msr	msp, r0		     /* MSP = exception_stack top */
	mov	r1, #2
	msr	control, r1	     /* CONTROL = 2 */
	bx	lr
