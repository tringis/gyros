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
#ifndef INCLUDE__interrupt_h__200808291459
#define INCLUDE__interrupt_h__200808291459

#include <gyros/arm/interrupt.h>

#define GYROS_IRQ_WDG          0
#define GYROS_IRQ_SW           1
#define GYROS_IRQ_ARMRX        2
#define GYROS_IRQ_ARMTX        3
#define GYROS_IRQ_TIM0         4
#define GYROS_IRQ_TIM1         5
#define GYROS_IRQ_TIM2         6
#define GYROS_IRQ_TIM3         7
#define GYROS_IRQ_USBHP        8
#define GYROS_IRQ_USBLP        9
#define GYROS_IRQ_SCU         10
#define GYROS_IRQ_EMAC        11
#define GYROS_IRQ_DMA         12
#define GYROS_IRQ_CAN         13
#define GYROS_IRQ_MC          14
#define GYROS_IRQ_ADC         15

#define GYROS_IRQ_UART0       16
#define GYROS_IRQ_UART1       17
#define GYROS_IRQ_UART2       18
#define GYROS_IRQ_I2C0        19
#define GYROS_IRQ_I2C1        20
#define GYROS_IRQ_SSP0        21
#define GYROS_IRQ_SSP1        22
#define GYROS_IRQ_LVD         23
#define GYROS_IRQ_RTC         24
#define GYROS_IRQ_WIU         25
#define GYROS_IRQ_EXTIT0      26
#define GYROS_IRQ_EXTIT1      27
#define GYROS_IRQ_EXTIT2      28
#define GYROS_IRQ_EXTIT3      29
#define GYROS_IRQ_USBWU       30
#define GYROS_IRQ_PFQBC       31

void gyros_target_set_isr(int irq, void (*isr)(void));

#endif
