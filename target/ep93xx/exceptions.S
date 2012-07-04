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

#include <gyros/arch/arm/context.S>

/**************************************************************************
 * Constants                                                              *
 **************************************************************************/	
	.equ	VIC1_CurVectAddr,	0x800B0030
	.equ	VIC2_CurVectAddr,	0x800C0030

/**************************************************************************
 * Functions                                                              *
 **************************************************************************/	
	.arm
	.text
	.balign		4

/*------------------------------------------------------------------------*
 | gyros__svc_handler - handles software interrupts as a means of forced  |
 |                      context switches (unless the scheduler is locked) |
 *------------------------------------------------------------------------*/
	.type	gyros__svc_handler, %function
	.global	gyros__svc_handler
gyros__svc_handler:
	add	lr, lr, #4
	save_context
	restore_context

/*------------------------------------------------------------------------*
 | gyros__irq_handler - handles interrupts                                |
 *------------------------------------------------------------------------*/
	.type	gyros__irq_handler, %function
	.global	gyros__irq_handler
gyros__irq_handler:
	save_context
	ldr	r0, =gyros__isr
	mov	lr, pc
	bx	r0
	restore_context
