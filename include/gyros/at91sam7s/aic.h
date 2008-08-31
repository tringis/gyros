#ifndef INCLUDED__aic_h__112128919811112399
#define INCLUDED__aic_h__112128919811112399

#include <stdint.h>

/* Internal interrupts: */
#define AIC_IRQ_MODE_INT_LEVEL      (0 << 5)
#define AIC_IRQ_MODE_INT_EDGE       (1 << 5)

/* External interrupt modes: */
#define AIC_IRQ_MODE_LOW_LEVEL      (0 << 5)
#define AIC_IRQ_MODE_NEG_EDGE       (1 << 5)
#define AIC_IRQ_MODE_HIGH_LEVEL     (2 << 5)
#define AIC_IRQ_MODE_POS_EDGE       (3 << 5)

typedef void (aic_isr_t)(void);

/* Initialize the AIC, disable all interrupts in the AIC but enable
 * interrupts in the ARM core. */
void aic_init(void);

/* Set the priority of an interrupt. */
int aic_irq_set_prio(int irq, int prio);

/* Add an interrupt handler. The function isr is a normal C function
 * with normal prologue and epilogue and calling convention.  The
 * interrupt must be enabled with aic_irq_enable(). */
int aic_irq_set_isr(int irq, int mode, aic_isr_t isr);

/* Add a system interrupt handler.  Because the system interrupt is
 * shared by several peripherals, we need to have several handlers.
 * The function isr is a normal C function with normal prologue and
 * epilogue and calling convention.  The interrupt must be enabled
 * with aic_irq_enable(). */
int aic_irq_add_sys_isr(aic_isr_t isr);

#endif
