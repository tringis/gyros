	.equ	VIC0_VAR,	0xFFFFF030
	.equ	VIC1_VAR,	0xFC000030

	.equ    MODE_USR,	0x10
	.equ    MODE_FIQ,	0x11
	.equ    MODE_IRQ,	0x12
	.equ    MODE_SVC,	0x13
	.equ    MODE_ABT,	0x17
	.equ    MODE_UND,	0x1b
	.equ    MODE_SYS,	0x1f

	.equ    FIQ_BIT,	0x40
	.equ    IRQ_BIT,	0x80

	/* Stack Sizes */
	.set  UND_STACK_SIZE,	0x00000020
	.set  ABT_STACK_SIZE,	0x00000020
	.set  FIQ_STACK_SIZE,	0x00000004
	.set  IRQ_STACK_SIZE,	0X00001000
	.set  SVC_STACK_SIZE,	0x00002000

.macro save_context
	/* Push lr to the SVC mode stack */
	stmdb		sp!, {lr}
	/* Load address of current task_t to lr */
	ldr		lr, =gyros_current_task
	ldr		lr, [lr]
	/* Push SYS mode r0-lr to current task_t */
	stmia		lr, {r0-lr}^
	nop
	mrs		r0, spsr
	str		r0, [lr, #16*4]
	/* Pop lr from the SVC mode stack and write it to the current task_t */
	ldmia		sp!, {r0}
	str		r0, [lr, #15*4]
.endm

.macro restore_context
	// Set the lr to the address of the task at the head of the
	// running list.
	ldr		r1, =gyros_running
	ldr		r0, [r1]
	sub		lr, r0, #17*4
	ldr		r1, =gyros_current_task
	str		lr, [r1]
	/* Get the SPSR from the task struct. */ 
	ldr		r0, [lr, #16*4]
	msr		spsr_cxsf, r0
	/* Restore all system mode registers for the task. */ 
	ldmia		lr, {r0-r14}^
	nop
	/* Restore the return address. */ 
	ldr		lr, [lr, #15*4]
	/* And return - correcting the offset in the LR to obtain the */ 
	/* correct address. */ 
	subs		pc, lr, #4
.endm

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
	.word	__swi_handler
	.word	__abort_prefetch_handler
	.word	__abort_data_handler
	.word	0
	.word	__irq_handler
	.word	__fiq_handler

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

	// Call str912_hwinit
	bl	str912_hwinit

#ifdef __thumb__
	// Switch back to ARM mode
	adr	r0, 1f
	bx	r0
	.align
	.arm
1:
#endif

	// Setup stack pointers
	ldr	r0, =__stack_top
	msr	cpsr_c, #MODE_UND | IRQ_BIT | FIQ_BIT
	mov	sp, r0
	sub	r0, r0, #UND_STACK_SIZE
	msr	cpsr_c, #MODE_ABT | IRQ_BIT | FIQ_BIT
	mov	sp, r0
	sub	r0, r0, #ABT_STACK_SIZE
	msr	cpsr_c, #MODE_FIQ | IRQ_BIT | FIQ_BIT
	mov	sp, r0
	sub	r0, r0, #FIQ_STACK_SIZE
	msr	cpsr_c, #MODE_IRQ | IRQ_BIT | FIQ_BIT
	mov	sp, r0
	sub	r0, r0, #IRQ_STACK_SIZE
	msr	cpsr_c, #MODE_SVC | IRQ_BIT | FIQ_BIT
	mov	sp, r0
	sub	r0, r0, #SVC_STACK_SIZE
	msr	cpsr_c, #MODE_SYS | IRQ_BIT | FIQ_BIT
	mov	sp, r0

	// Go back to supervisor mode with IRQ and FIQ disabled
	msr	cpsr_c, #IRQ_BIT | FIQ_BIT | MODE_SVC

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

	.weak	__undefined_instruction_handler
	.type	__undefined_instruction_handler, %function
__undefined_instruction_handler:
	b	.

	.type	__swi_handler, %function
__swi_handler:
	add	lr, lr, #4
	save_context
	ldr	r0, =gyros__yield
	mov	lr, pc
	bx	r0
	restore_context

	.weak	__abort_prefetch_handler
	.type	__abort_prefetch_handler, %function
__abort_prefetch_handler:
	b	.

	.weak	__abort_data_handler
	.type	__abort_data_handler, %function
__abort_data_handler:
	b	.

	.type	__irq_handler, %function
__irq_handler:
	save_context
	ldr	r0, =VIC0_VAR
	ldr	r0, [r0]
	ldr	r1, =VIC1_VAR
	ldr	r1, [r1]
	mov	lr, pc
	bx	r0
	/* Write to VIC0_VAR and VIC1_VAR to signal end of ISR */
	ldr	r0, =VIC0_VAR
	str	r0, [r0]
	ldr	r1, =VIC1_VAR
	str	r1, [r1]
	restore_context

	.weak	__fiq_handler
	.type	__fiq_handler, %function
__fiq_handler:
	b	.

	.weak	__gccmain
	.type	__gccmain, %function
__gccmain:
	mov	pc, lr
