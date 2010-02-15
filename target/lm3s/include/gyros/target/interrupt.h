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
#ifndef INCLUDE__gyros_lm3s_interrupt_h__201002062211
#define INCLUDE__gyros_lm3s_interrupt_h__201002062211

#include <gyros/arch/armv7-m/interrupt.h>

#define LM3S_IRQ_GPIO_A                   0
#define LM3S_IRQ_GPIO_B                   1
#define LM3S_IRQ_GPIO_C                   2
#define LM3S_IRQ_GPIO_D                   3
#define LM3S_IRQ_GPIO_E                   4
#define LM3S_IRQ_UART0                    5
#define LM3S_IRQ_UART1                    6
#define LM3S_IRQ_SSI0                     7
#define LM3S_IRQ_I2C0                     8
#define LM3S_IRQ_PWM_FAULT                9
#define LM3S_IRQ_PWM_GENERATOR_0         10
#define LM3S_IRQ_PWM_GENERATOR_1         11
#define LM3S_IRQ_PWM_GENERATOR_2         12
#define LM3S_IRQ_QEI0                    13
#define LM3S_IRQ_ADC0_SEQUENCE_0         14
#define LM3S_IRQ_ADC0_SEQUENCE_1         15
#define LM3S_IRQ_ADC0_SEQUENCE_2         16
#define LM3S_IRQ_ADC0_SEQUENCE_3         17
#define LM3S_IRQ_WATCHDOG_TIMERS_0_AND_1 18
#define LM3S_IRQ_TIMER_0A                19
#define LM3S_IRQ_TIMER_0B                20
#define LM3S_IRQ_TIMER_1A                21
#define LM3S_IRQ_TIMER_1B                22
#define LM3S_IRQ_TIMER_2A                23
#define LM3S_IRQ_TIMER_2B                24
#define LM3S_IRQ_ANALOG_COMPARATOR_0     25
#define LM3S_IRQ_ANALOG_COMPARATOR_1     26
#define LM3S_IRQ_ANALOG_COMPARATOR_2     27
#define LM3S_IRQ_SYSTEM_CONTROL          28
#define LM3S_IRQ_FLASH_MEMORY_CONTROL    29
#define LM3S_IRQ_GPIO_PORT_F             30
#define LM3S_IRQ_GPIO_PORT_G             31
#define LM3S_IRQ_GPIO_PORT_H             32
#define LM3S_IRQ_UART2                   33
#define LM3S_IRQ_SSI1                    34
#define LM3S_IRQ_TIMER_3A                35
#define LM3S_IRQ_TIMER_3B                36
#define LM3S_IRQ_I2C1                    37
#define LM3S_IRQ_QEI1                    38
#define LM3S_IRQ_CAN0                    39
#define LM3S_IRQ_CAN1                    40
#define LM3S_IRQ_RESERVED_41             41
#define LM3S_IRQ_ETHERNET_CONTROLLER     42
#define LM3S_IRQ_RESERVED_43             43
#define LM3S_IRQ_USB                     44
#define LM3S_IRQ_PWM_GENERATOR_3         45
#define LM3S_IRQ_UDMA_SOFTWARE           46
#define LM3S_IRQ_UDMA_ERROR              47
#define LM3S_IRQ_ADC1_SEQUENCE_0         48
#define LM3S_IRQ_ADC1_SEQUENCE_1         49
#define LM3S_IRQ_ADC1_SEQUENCE_2         50
#define LM3S_IRQ_ADC1_SEQUENCE_3         51
#define LM3S_IRQ_I2S0                    52
#define LM3S_IRQ_EPI                     53
#define LM3S_IRQ_GPIO_PORT_J             54

#endif
