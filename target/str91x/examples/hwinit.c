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

#include "../str91x.h"

struct FMI_regs
{
    reg32 BBSR;
    reg32 NBBSR;
    reg32 EMPTY1;
    reg32 BBADR;
    reg32 NBBADR;
    reg32 EMPTY2;
    reg32 CR;
    reg32 SR;
    reg32 BCE5ADDR;
};

#define FMI          ((struct FMI_regs*)0x54000000)

/* WARNING: This function is run before the data and bss sections are
 * set up, so it cannot access global or static variables. */
void
gyros__hwinit(void)
{
    /* Setup and enable flash memory banks */
    FMI->BBSR = 0x00000004;
    FMI->BBADR = 0x00000000;
    FMI->NBBSR = 0x00000002;
    FMI->NBBADR = 0x00020000;
    FMI->CR = 0x00000018;

    /* Configure 2 read wait states for the flash */
    *(reg16*)0x00080000 = 0x60;
    *(reg16*)0x00083040 = 0x03;

    /* Enable 96 KB SRAM */
    SCU->SCR0 |= (2U << 3);

    /* Switch CPU master clock source to OSC */
    SCU->CLKCNTR = (1U << 1);

    /* Configure PLL for 96 MHz (P=2, N=192, M=25) */
    SCU->PLLCONF = (2U << 16) | (192U << 8) | 25U;

    /* Enable the PLL */
    SCU->PLLCONF |= 1U << 19;

    /* Wait for PLL to lock */
    while ((SCU->SYSSTATUS & 1) == 0)
        ;

    /* Switch CPU master clock source to PLL, set PCLK=RCLK/2 */
    SCU->CLKCNTR = (1U << 7);

    /* Enable VIC */
    SCU->PCGR0 |= SCU_P0_VIC;
    SCU->PRR0 |= SCU_P0_VIC;
}