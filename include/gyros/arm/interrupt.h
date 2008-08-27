#ifndef INCLUDE__arm7tdmi_interrupt_h__200212292232
#define INCLUDE__arm7tdmi_interrupt_h__200212292232

/* Enable interrupts in the ARM core. */
static __inline__ void
gyros_interrupts_enable(void)
{
	unsigned long temp;

	/* Inline assembly to clear the IRQ and FIQ bits in CPSR. */
	__asm__ __volatile__(
		"mrs	%0, cpsr\n\t"
		"bic	%0, %0, #0xc0\n\t"
		"msr	cpsr_c, %0"
		: "=r" (temp) :: "memory");
}

/* Disable interrupts in the ARM core. */
static __inline__ void
gyros_interrupts_disable(void)
{
	unsigned long temp;

	/* Inline assembly to set the IRQ bit in CPSR. */
	__asm__ __volatile__(
		"mrs	%0, cpsr\n\t"
		"orr	%0, %0, #0xc0\n\t"
		"msr	cpsr_c, %0"
		: "=r" (temp) :: "memory");
}

/* Yield. */
static __inline void
gyros_yield(void)
{
	__asm__ __volatile__("swi" ::: "memory");
}

#endif
