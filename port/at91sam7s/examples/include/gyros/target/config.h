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
#ifndef INCLUDED__gyros_at91sam7s_config_h__200901021029
#define INCLUDED__gyros_at91sam7s_config_h__200901021029

/*---------------------------------------------------------------------*
 * Application specific configuration  (see gyros/target/defconfig.h
 *                                      and gyros/private/defconfig.h)
 *---------------------------------------------------------------------*/
#define GYROS_CONFIG_DEBUG                     1

#include <gyros/target/defconfig.h>

/*---------------------------------------------------------------------*
 * GyrOS target specific configuration
 *---------------------------------------------------------------------*/
#define GYROS_CONFIG_AT91SAM7S_MCLK            47923200

#if GYROS_CONFIG_DYNTICK

#define GYROS_CONFIG_TIMER_NUM             2
#define GYROS_CONFIG_TIMER_CLK             AT91C_TC_CLKS_TIMER_DIV3_CLOCK
#define GYROS_CONFIG_TIMER_RESOLUTION      (GYROS_CONFIG_AT91SAM7S_MCLK / 32)

#define GYROS_CONFIG_US_TO_TICKS(us)       ((long long)(us) * GYROS_CONFIG_TIMER_RESOLUTION / 1000000)
#define GYROS_CONFIG_MS_TO_TICKS(ms)       ((long long)(ms) * GYROS_CONFIG_TIMER_RESOLUTION / 1000)
#define GYROS_CONFIG_S_TO_TICKS(s)         ((long long)(s) * GYROS_CONFIG_TIMER_RESOLUTION)

#define GYROS_CONFIG_TICKS_TO_US(us)       ((long long)(us) * 1000000 / GYROS_CONFIG_TIMER_RESOLUTION)
#define GYROS_CONFIG_TICKS_TO_MS(ms)       ((long long)(ms) * 1000 / GYROS_CONFIG_TIMER_RESOLUTION)
#define GYROS_CONFIG_TICKS_TO_S(s)         ((long long)(s) / GYROS_CONFIG_TIMER_RESOLUTION)

#else /* GYROS_CONFIG_DYNTICK */

#define GYROS_CONFIG_TIMER_RESOLUTION      1000

#define GYROS_CONFIG_US_TO_TICKS(us)       ((us) / 1000)
#define GYROS_CONFIG_MS_TO_TICKS(ms)       (ms)
#define GYROS_CONFIG_S_TO_TICKS(s)         ((s) * 1000)

#define GYROS_CONFIG_TICKS_TO_US(us)       ((us) * 1000)
#define GYROS_CONFIG_TICKS_TO_MS(ms)       (ms)
#define GYROS_CONFIG_TICKS_TO_S(s)         ((s) / 1000)

#endif /* GYROS_CONFIG_DYNTICK */

#endif
