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
#include <at91sam7s.h>

#define MCLK_FREQ           47923200

/* WARNING: This function is run before the data and bss sections are
 * set up, so it cannot access global or static variables. */
void
gyros__hwinit(void)
{
    /* Configure the flash controller. */
    AT91C_BASE_MC->MC_FMR = (((15 * MCLK_FREQ + 9999999) / 10000000) << 16) |
        AT91C_MC_FWS_1FWS;

    /* Disable the watchdog */
    AT91C_BASE_WDTC->WDTC_WDMR = AT91C_SYSC_WDDIS;

    /* Enable the main oscillator */
    AT91C_BASE_PMC->PMC_MOR = (6U << 8) | AT91C_CKGR_MOSCEN;

    /* Wait for the main oscillator to stabilize */
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS))
        ;

    /* Configure the PLL */
    AT91C_BASE_PMC->PMC_PLLR = 5U | (16U << 8) | (25U << 16);

    /* Wait for the PLL to stabilize */
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCK))
        ;

    /* Configure the master clock prescaler */
    AT91C_BASE_PMC->PMC_MCKR =
        (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_PRES) | AT91C_PMC_PRES_CLK_2;

    /* Wait for the master clock to stabilize */
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
        ;

    /* Configure the master clock to use the PLL clock */
    AT91C_BASE_PMC->PMC_MCKR =
        (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_CSS) | AT91C_PMC_CSS_PLL_CLK;

    /* Wait for the master clock to stabilize */
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
        ;
}
