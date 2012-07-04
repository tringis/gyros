/**************************************************************************
 * Copyright (c) 2002-2012, Tobias Ringström <tobias@ringis.se>
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
#include <gyros/interrupt.h>

#include "ep93xx.h"

typedef void (*isr_ptr_t)(void);

static isr_ptr_t s_vic1_isr[32];
static isr_ptr_t s_vic2_isr[32];

static void
handle_vic(struct VIC_regs *vic, const isr_ptr_t *isrs)
{
    unsigned status = vic->IRQStatus, i;
    if (status == 0)
        return;
    for (i = 0; i < 32; ++i)
    {
        if (status & (1U << i))
            isrs[i]();
    }
}

void
gyros__isr(void)
{
#if GYROS_CONFIG_IRQ_HOOK || GYROS_CONFIG_TRACE
	void gyros__irq_hook();
	gyros__irq_hook();
#endif
    handle_vic(VIC1, s_vic1_isr);
    handle_vic(VIC2, s_vic2_isr);
}

void
gyros_target_set_isr(int irq, void (*isr)(void))
{
    unsigned long flags = gyros_interrupt_disable();
    struct VIC_regs *vic;
    isr_ptr_t *isr_tab;
        
    if (irq < 32)
    {
        vic = VIC1;
        isr_tab = s_vic1_isr;
    }
    else
    {
        vic = VIC2;
        isr_tab = s_vic2_isr;
    }
    irq &= 31;

    vic->IntEnClear = 1 << irq; /* Disable the interrupt */
    if (isr)
    {
        isr_tab[irq] = isr;
        vic->IntEnable = 1 << irq; /* Enable the interrupt */
    }
    gyros_interrupt_restore(flags);
}
