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

#include "../private.h"
#include "at91sam7s.h"

#define MCLK_FREQ    47923200

#define PIT_FREQ     (MCLK_FREQ / 16)
#define PIT_PERIOD   (PIT_FREQ / 1000)  /* cycles per 1 ms */

unsigned long gyros__ticks;

static void
pit_isr(void)
{
    if (AT91C_BASE_PITC->PITC_PISR & AT91C_SYSC_PITS)
    {
        uint32_t status = AT91C_BASE_PITC->PITC_PIVR;

        gyros__ticks += status >> 20;
        gyros__wake_sleeping_tasks();
    }
}

void
gyros__tick_enable(void)
{
    uint32_t dummy;

    aic_irq_add_sys_isr(pit_isr);

    dummy = AT91C_BASE_PITC->PITC_PIVR;
    AT91C_BASE_PITC->PITC_PIMR = AT91C_SYSC_PITEN | AT91C_SYSC_PITIEN |
        (PIT_PERIOD - 1);

    *AT91C_AIC_IECR = (1 << AT91C_ID_SYS);
}
