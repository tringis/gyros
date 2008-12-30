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
#ifndef INCLUDED__gyros_at91sam7s_interrupt_h__200812301816
#define INCLUDED__gyros_at91sam7s_interrupt_h__200812301816

#include <stdint.h>

#include <gyros/arm/interrupt.h>

#define GYROS_IRQ_FIQ                0 // Advanced Interrupt Controller (FIQ)
#define GYROS_IRQ_SYS                1 // System Peripheral
#define GYROS_IRQ_PIOA               2 // Parallel IO Controller
#define GYROS_IRQ_ADC                4 // Analog-to-Digital Converter
#define GYROS_IRQ_SPI                5 // Serial Peripheral Interface
#define GYROS_IRQ_US0                6 // USART 0
#define GYROS_IRQ_US1                7 // USART 1
#define GYROS_IRQ_SSC                8 // Serial Synchronous Controller
#define GYROS_IRQ_TWI                9 // Two-Wire Interface
#define GYROS_IRQ_PWMC              10 // PWM Controller
#define GYROS_IRQ_UDP               11 // USB Device Port
#define GYROS_IRQ_TC0               12 // Timer Counter 0
#define GYROS_IRQ_TC1               13 // Timer Counter 1
#define GYROS_IRQ_TC2               14 // Timer Counter 2
#define GYROS_IRQ_IRQ0              30 // Advanced Interrupt Controller (IRQ0)
#define GYROS_IRQ_IRQ1              31 // Advanced Interrupt Controller (IRQ1)

/* Internal interrupts: */
#define GYROS_IRQ_MODE_INT_LEVEL    (0 << 5)
#define GYROS_IRQ_MODE_INT_EDGE     (1 << 5)

/* External interrupt modes: */
#define GYROS_IRQ_MODE_LOW_LEVEL    (0 << 5)
#define GYROS_IRQ_MODE_NEG_EDGE     (1 << 5)
#define GYROS_IRQ_MODE_HIGH_LEVEL   (2 << 5)
#define GYROS_IRQ_MODE_POS_EDGE     (3 << 5)

typedef void (gyros_target_aic_isr_t)(void);

/* Initialize the AIC, disable all interrupts in the AIC but enable
 * interrupts in the ARM core. */
void gyros_target_aic_init(void);

/* Set the priority of an interrupt. */
int gyros_target_set_irq_prio(int irq, int prio);

/* Add an interrupt handler. The function isr is a normal C function
 * with normal prologue and epilogue and calling convention.  The
 * interrupt must be enabled with aic_irq_enable(). */
int gyros_target_set_isr(int irq, int mode, gyros_target_aic_isr_t isr);

/* Add a system interrupt handler.  Because the system interrupt is
 * shared by several peripherals, we need to have several handlers.
 * The function isr is a normal C function with normal prologue and
 * epilogue and calling convention.  The interrupt must be enabled
 * with aic_irq_enable(). */
int gyros_target_add_sys_isr(gyros_target_aic_isr_t isr);

#endif
