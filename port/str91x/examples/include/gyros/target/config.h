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
#ifndef INCLUDED__gyros_at91sam7s_config_h__200901021701
#define INCLUDED__gyros_at91sam7s_config_h__200901021701

/*---------------------------------------------------------------------*
 * Application specific configuration  (see gyros/private/defconfig.h)
 *---------------------------------------------------------------------*/
#define GYROS_CONFIG_DEBUG                     1

#define GYROS_CONFIG_DYNTICK                   0

#include <gyros/target/defconfig.h>

/*---------------------------------------------------------------------*
 * GyrOS target specific configuration
 *---------------------------------------------------------------------*/
#ifdef __THUMB_INTERWORK__
#define GYROS_CONFIG_THUMB                     1
#else
#define GYROS_CONFIG_THUMB                     0
#endif

#define GYROS_CONFIG_STR91X_PCLK               48000000

#define GYROS_CONFIG_TIMER_RESOLUTION          1000000

#define GYROS_CONFIG_US_TO_TICKS(us)           (us)
#define GYROS_CONFIG_MS_TO_TICKS(ms)           ((ms) * 1000)
#define GYROS_CONFIG_S_TO_TICKS(s)             ((s) * 1000000)

#define GYROS_CONFIG_TICKS_TO_US(us)           (us)
#define GYROS_CONFIG_TICKS_TO_MS(ms)           ((ms) / 1000)
#define GYROS_CONFIG_TICKS_TO_S(s)             ((s) / 1000000)

#endif
