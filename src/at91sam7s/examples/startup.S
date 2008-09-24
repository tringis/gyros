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
	.arm
	.section	.text.vectors, "ax"
	.balign		4

	.global __real_exception_vectors
__real_exception_vectors:
	ldr	pc, [pc, #0x18]		// __reset_handler		       
	ldr	pc, [pc, #0x18]		// __undefined_instruction_handler
	ldr	pc, [pc, #0x18]		// __swi_handler		       
	ldr	pc, [pc, #0x18]		// __abort_prefetch_handler	  
	ldr	pc, [pc, #0x18]		// __abort_data_handler
	b	.			// RESERVED
	ldr	pc, [pc, #0x18]		// AIC_IVR
	ldr	pc, [pc, #0x18]		// AIC_FVR

	.global		__exception_vectors
	.type		__exception_vectors, %object
__exception_vectors:
	.word	gyros__reset_handler
	.word	__undefined_instruction_handler
	.word	gyros__swi_handler
	.word	__abort_prefetch_handler
	.word	__abort_data_handler
	.word	0
	.word	gyros__irq_handler
	.word	__fiq_handler

	.weak	__undefined_instruction_handler
	.type	__undefined_instruction_handler, %function
__undefined_instruction_handler:
	b	.

	.weak	__abort_prefetch_handler
	.type	__abort_prefetch_handler, %function
__abort_prefetch_handler:
	b	.

	.weak	__abort_data_handler
	.type	__abort_data_handler, %function
__abort_data_handler:
	b	.

	.weak	__fiq_handler
	.type	__fiq_handler, %function
__fiq_handler:
	b	.
