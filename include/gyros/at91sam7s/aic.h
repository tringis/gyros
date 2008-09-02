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
