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
#ifndef INCLUDED__gyros_str91x_h__200808312206
#define INCLUDED__gyros_str91x_h__200808312206

#include <stdint.h>

typedef volatile uint16_t reg16;
typedef volatile uint32_t reg32;

/* AHB peripherals */
#define SCU_P0_FMI             (1U <<  0)
#define SCU_P0_FPQBC           (1U <<  1)
#define SCU_P0_SRAM            (1U <<  3)
#define SCU_P0_SRAM_ARBITER    (1U <<  4)
#define SCU_P0_VIC             (1U <<  5)
#define SCU_P0_EMI             (1U <<  6)
#define SCU_P0_EMI_MEM_CLK     (1U <<  7)
#define SCU_P0_DMA             (1U <<  8)
#define SCU_P0_USB             (1U <<  9)
#define SCU_P0_USB48M          (1U << 10)
#define SCU_P0_MAC             (1U << 11)
#define SCU_P0_PFQBC_AHB       (1U << 12)

/* APB peripherals */
#define SCU_P1_TIM01           (1U <<  0)
#define SCU_P1_TIM23           (1U <<  1)
#define SCU_P1_MC              (1U <<  2)
#define SCU_P1_UART0           (1U <<  3)
#define SCU_P1_UART1           (1U <<  4)
#define SCU_P1_UART2           (1U <<  5)
#define SCU_P1_I2C0            (1U <<  6)
#define SCU_P1_I2C1            (1U <<  7)
#define SCU_P1_SSP0            (1U <<  8)
#define SCU_P1_SSP1            (1U <<  9)
#define SCU_P1_CAN             (1U << 10)
#define SCU_P1_ADC             (1U << 11)
#define SCU_P1_WDG             (1U << 12)
#define SCU_P1_WIU             (1U << 13)
#define SCU_P1_GPIO0           (1U << 14)
#define SCU_P1_GPIO1           (1U << 15)
#define SCU_P1_GPIO2           (1U << 16)
#define SCU_P1_GPIO3           (1U << 17)
#define SCU_P1_GPIO4           (1U << 18)
#define SCU_P1_GPIO5           (1U << 19)
#define SCU_P1_GPIO6           (1U << 20)
#define SCU_P1_GPIO7           (1U << 21)
#define SCU_P1_GPIO8           (1U << 22)
#define SCU_P1_GPIO9           (1U << 23)
#define SCU_P1_RTC             (1U << 24)

struct SCU_regs
{
    reg32 CLKCNTR;
    reg32 PLLCONF;
    reg32 SYSSTATUS;
    reg32 PWRMNG;
    reg32 ITCMSK;
    reg32 PCGR0;
    reg32 PCGR1;
    reg32 PRR0;
    reg32 PRR1;
    reg32 MGR0;
    reg32 MGR1;
    reg32 PECGR0;
    reg32 PECGR1;
    reg32 SCR0;
    reg32 SCR1;
    reg32 SCR2;
    uint32_t reserved1;
    reg32 GPIOOUT[8];
    reg32 GPIOIN[8];
    reg32 GPIOTYPE[10];
    reg32 GPIOEMI;
    reg32 WKUPSEL;
    uint32_t reserved2[2];
    reg32 GPIOANA;
};

struct TIM_regs
{
    reg16 IC1R;
    reg16 EMPTY1;
    reg16 IC2R;
    reg16 EMPTY2;
    reg16 OC1R;
    reg16 EMPTY3;
    reg16 OC2R;
    reg16 EMPTY4;
    reg16 CNTR;
    reg16 EMPTY5;
    reg16 CR1;
    reg16 EMPTY6;
    reg16 CR2;
    reg16 EMPTY7;
    reg16 SR;
    reg16 EMPTY8;
};

#define TIM_CR2_IC1IE        (1U << 15)
#define TIM_CR2_OC1IE        (1U << 14)
#define TIM_CR2_TOIE         (1U << 13)
#define TIM_CR2_IC2IE        (1U << 12)
#define TIM_CR2_OC2IE        (1U << 11)
#define TIM_CR2_DMAE         (1U << 10)

#define TIM_SR_ICF1          (1U << 15)
#define TIM_SR_OCF1          (1U << 14)
#define TIM_SR_TOF           (1U << 13)
#define TIM_SR_ICF2          (1U << 12)
#define TIM_SR_OCF2          (1U << 11)

struct VIC_regs
{
    reg32 ISR;
    reg32 FSR;
    reg32 RINTSR;
    reg32 INTSR;
    reg32 INTER;
    reg32 INTECR;
    reg32 SWINTR;
    reg32 SWINTCR;
    reg32 PER;
    reg32 EMPTY1[3];
    reg32 VAR;
    reg32 DVAR;
    reg32 EMPTY2[50];
    reg32 VAiR[16];
    reg32 EMPTY3[48];
    reg32 VCiR[16];
};

/*----------------------------------------------------------------------------*/

#define SCU          ((struct SCU_regs*)0x5c002000)
#define TIM(n)       ((struct TIM_regs*)(0x58002000 + 0x1000 * (n)))
#define VIC0         ((struct VIC_regs*)0xfffff000)
#define VIC1         ((struct VIC_regs*)0xfc000000)

#endif
