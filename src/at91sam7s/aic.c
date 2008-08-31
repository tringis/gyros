#include <gyros/at91sam7s/aic.h>

#include <gyros/arm/interrupt.h>

#include "at91sam7s.h"

#define MAX_SYS_ISR_COUNT 4

extern void aic_spurious_irq_handler(void);
extern void aic_isr(void);

aic_isr_t *aic_isr_table[32];

static int s_sys_isr_count;
static aic_isr_t *s_sys_isr_table[MAX_SYS_ISR_COUNT];

static void
aic_sys_isr(void)
{
    int i;

    for (i = 0; i < s_sys_isr_count; ++i)
        s_sys_isr_table[i]();
}

static void
aic_spurious_isr(void)
{
}

void aic_init(void)
{
    int i;

    for (i = 0; i < 32; ++i)
        AT91C_AIC_SVR[i] = 0;
    *AT91C_AIC_SPU = (uint32_t)aic_spurious_isr;

    *AT91C_AIC_IDCR = -1; /* Disable all AIC interrupts */
    *AT91C_AIC_EOICR = 0; /* End any pending interrupts */

    aic_irq_set_isr(AT91C_ID_SYS, AIC_IRQ_MODE_INT_LEVEL, aic_sys_isr);
}

int aic_irq_set_prio(int irq, int prio)
{
    if (prio < 0 || prio > 7)
        return 0;

    AT91C_AIC_SMR[irq] = (AT91C_AIC_SMR[irq] & ~AT91C_AIC_PRIOR) | prio;

    return 1;
}

int aic_irq_set_isr(int irq, int mode, aic_isr_t isr)
{
    AT91C_AIC_SMR[irq] = (AT91C_AIC_SMR[irq] & AT91C_AIC_PRIOR) | mode;
    aic_isr_table[irq] = isr;

    return 1;
}

int aic_irq_add_sys_isr(aic_isr_t isr)
{
    unsigned long flags = gyros_interrupt_disable();

    if (s_sys_isr_count >= MAX_SYS_ISR_COUNT)
    {
        gyros_interrupt_restore(flags);
        return 0;
    }

    s_sys_isr_table[s_sys_isr_count++] = isr;
    gyros_interrupt_restore(flags);

    return 1;
}
