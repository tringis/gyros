#ifndef INCLUDE__arm7tdmi_interrupt_h__200212292232
#define INCLUDE__arm7tdmi_interrupt_h__200212292232

#include <gyros/arm/arm_defs.h>

/* Disable interrupts (IRQ and FIQ) in the ARM core. */
static __inline__ unsigned long
gyros_interrupt_disable(void)
{
    unsigned long temp, flags;

    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "mrs    %1, cpsr\n\t"
        "orr    %0, %1, #0xc0\n\t"
        "msr    cpsr_c, %0\n\t"
        : "=r" (temp), "=&r" (flags) :: "memory");

    return flags;
}

/* Restore interrupts (IRQ and FIQ) in the ARM core. */
static __inline__ void
gyros_interrupt_restore(unsigned long flags)
{
    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "msr    cpsr_c, %0\n\t"
        :: "r" (flags) : "memory");
}

/* Enable interrupts in the ARM core. */
static __inline__ void
gyros__interrupt_enable(void)
{
    unsigned long temp;

    /* Inline assembly to clear the IRQ and FIQ bits in CPSR. */
    __asm__ __volatile__(
        "mrs    %0, cpsr\n\t"
        "bic    %0, %0, #0xc0\n\t"
        "msr    cpsr_c, %0"
        : "=r" (temp) :: "memory");
}

static __inline__ int
gyros_in_interrupt(void)
{
    unsigned long cpsr;

    /* Inline assembly to set the IRQ bit in CPSR. */
    __asm__ __volatile__(
        "mrs    %0, cpsr\n\t"
        : "=r" (cpsr) :: "memory");

    return (cpsr & 0x1f) != ARM_MODE_SYS;
}

/* Reschedule, i.e. make sure the right task is running. */
static __inline void
gyros__reschedule(void)
{
    if (!gyros_in_interrupt())
        __asm__ __volatile__("swi" ::: "memory");
}

#endif
