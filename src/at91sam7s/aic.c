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
