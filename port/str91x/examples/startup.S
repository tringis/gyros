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
	.equ    FIQ_BIT,	0x40
	.equ    IRQ_BIT,	0x80

	.equ    MODE_USR,	0x10
	.equ    MODE_FIQ,	0x11
	.equ    MODE_IRQ,	0x12
	.equ    MODE_SVC,	0x13
	.equ    MODE_ABT,	0x17
	.equ    MODE_UND,	0x1b
	.equ    MODE_SYS,	0x1f

	/* Stack Sizes */
	.equ  UND_STACK_SIZE,		4
	.equ  ABT_STACK_SIZE,		4
	.equ  FIQ_STACK_SIZE,		4
	.equ  IRQ_SVC_STACK_SIZE,	512

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
	.word	__reset_handler
	.word	__undefined_instruction_handler
	.word	gyros__swi_handler
	.word	__abort_prefetch_handler
	.word	__abort_data_handler
	.word	0
	.word	gyros__irq_handler
	.word	__fiq_handler

	.type	__undefined_instruction_handler, %function
__undefined_instruction_handler:
	b	.

	.type	__abort_prefetch_handler, %function
__abort_prefetch_handler:
	b	.

	.type	__abort_data_handler, %function
__abort_data_handler:
	b	.

	.type	__fiq_handler, %function
__fiq_handler:
	b	.

	.arm
	.text
	.balign		4

	.global	__reset_handler
__reset_handler:
	// Go to supervisor mode with IRQ and FIQ disabled
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_SVC

	// Setup main stack pointer
	ldr	sp, =__hwinit_stack_top

#ifdef __thumb__
	// Switch to thumb mode
	adr	r0, 1f + 1
	bx	r0
	.thumb
1:
#endif

	// Call gyros__hwinit
	bl	gyros__hwinit

#ifdef __thumb__
	// Switch back to ARM mode
	adr	r0, 1f
	bx	r0
	.align
	.arm
1:
#endif

	// Setup stack pointers and go to system mode
	ldr	r0, =__stack_top
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_UND
	mov	sp, r0
	sub	r0, r0, #UND_STACK_SIZE
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_ABT
	mov	sp, r0
	sub	r0, r0, #ABT_STACK_SIZE
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_FIQ
	mov	sp, r0
	sub	r0, r0, #FIQ_STACK_SIZE
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_IRQ
	mov	sp, r0
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_SVC
	mov	sp, r0
	sub	r0, r0, #IRQ_SVC_STACK_SIZE
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_SYS
	mov	sp, r0

	// Copy .text.ram section
	ldr	r0, =__text_ram_start
	ldr	r1, =__text_ram_load
	ldr	r2, =__text_ram_size
	bl	__crt_memcpy

	// Copy .data section
	ldr	r0, =__data_start
	ldr	r1, =__data_load
	ldr	r2, =__data_size
	bl	__crt_memcpy

	// Zero .bss
	mov	r0, #0
	ldr	r1, =__bss_start
	ldr	r2, =__bss_size
	cmp	r0, r2
	beq	2f
1:	subs	r2, r2, #4
	str	r0, [r1], #4
	bne	1b
2:

#ifdef __thumb__
	// Switch to thumb mode
	adr	r0, 1f + 1
	bx	r0
	.thumb
1:
#endif

	// Call main
	mov	r0, #0
	bl	main

	// Loop forever
	b	.

	// ARM code below
	.align
	.arm

	// __crt_memcpy:
	// r0 - destination address
	// r1 - source address
	// r2 - size in bytes (Must be a multiple of 4)
	.type	__crt_memcpy, %function
__crt_memcpy:
	subs	r2, r2, #32
	bmi	2f		// r0 was < 32
1:	ldmfd	r1!, {r3-r10}
	subs	r2, r2, #32
	stmea	r0!, {r3-r10}
	bpl	1b
2:	adds	r2, r2, #32
	moveq	pc, lr          // r0 was a multiple of 32
1:	ldr	r3, [r1], #4    // Copy the rest
	subs	r2, r2, #4
	str	r3, [r0], #4
	bne	1b
	mov	pc, lr

	.weak	__gccmain
	.type	__gccmain, %function
__gccmain:
	mov	pc, lr
