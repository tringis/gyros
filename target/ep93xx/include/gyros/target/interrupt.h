/**************************************************************************
 * Copyright (c) 2002-2009, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDE__gyros_str91x_interrupt_h__200808291459
#define INCLUDE__gyros_str91x_interrupt_h__200808291459

#include <gyros/arch/arm/interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYROS_IRQ_COMMRX             2
#define GYROS_IRQ_COMMTX             3
#define GYROS_IRQ_TC1UI              4
#define GYROS_IRQ_TC2UI              5
#define GYROS_IRQ_AACINTR            6
#define GYROS_IRQ_DMAM2P0            7
#define GYROS_IRQ_DMAM2P1            8
#define GYROS_IRQ_DMAM2P2            9
#define GYROS_IRQ_DMAM2P3           10
#define GYROS_IRQ_DMAM2P4           11
#define GYROS_IRQ_DMAM2P5           12
#define GYROS_IRQ_DMAM2P6           13
#define GYROS_IRQ_DMAM2P7           14
#define GYROS_IRQ_DMAM2P8           15
#define GYROS_IRQ_DMAM2P9           16
#define GYROS_IRQ_DMAM2M0           17
#define GYROS_IRQ_DMAM2M1           18
#define GYROS_IRQ_UART1RXINTR1      23
#define GYROS_IRQ_UART1TXINTR1      24
#define GYROS_IRQ_UART2RXINTR2      25
#define GYROS_IRQ_UART2TXINTR2      26
#define GYROS_IRQ_UART3RXINTR3      27
#define GYROS_IRQ_UART3TXINTR3      28
#define GYROS_IRQ_INT_KEY           29
#define GYROS_IRQ_INT_TOUCH         30
#define GYROS_IRQ_INT_EXT0          32
#define GYROS_IRQ_INT_EXT1          33
#define GYROS_IRQ_INT_EXT2          34
#define GYROS_IRQ_TINTR             35
#define GYROS_IRQ_WEINT             36
#define GYROS_IRQ_INT_RTC           37
#define GYROS_IRQ_INT_IrDA          38
#define GYROS_IRQ_INT_MAC           39
#define GYROS_IRQ_INT_PROG          41
#define GYROS_IRQ_CLK1HZ            42
#define GYROS_IRQ_V_SYNC            43
#define GYROS_IRQ_INT_VIDEO_FIFO    44
#define GYROS_IRQ_INT_SSP1RX        45
#define GYROS_IRQ_INT_SSP1TX        46
#define GYROS_IRQ_TC3UI             51
#define GYROS_IRQ_INT_UART1         52
#define GYROS_IRQ_SSPINTR           53
#define GYROS_IRQ_INT_UART2         54
#define GYROS_IRQ_INT_UART3         55
#define GYROS_IRQ_USHINTR           56
#define GYROS_IRQ_INT_PME           57
#define GYROS_IRQ_INT_DSP           58
#define GYROS_IRQ_GPIOINTR          59
#define GYROS_IRQ_I2SINTR           60

void gyros_target_set_isr(int irq, void (*isr)(void));

#ifdef __cplusplus
}
#endif

#endif
