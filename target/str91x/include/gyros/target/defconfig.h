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
#ifndef INCLUDED__gyros_str91x_defconfig_h__200901101016
#define INCLUDED__gyros_str91x_defconfig_h__200901101016

#ifndef GYROS_CONFIG_DYNTICK
#define GYROS_CONFIG_DYNTICK                   1
#endif

#ifndef GYROS_CONFIG_STR91X_TIMER
#define GYROS_CONFIG_STR91X_TIMER              3
#endif

#ifndef GYROS_CONFIG_STR91X_TIMER_HZ
#define GYROS_CONFIG_STR91X_TIMER_HZ           1000000
#endif

#ifndef GYROS_CONFIG_HZ
#if GYROS_CONFIG_DYNTICK
#  define GYROS_CONFIG_HZ                      GYROS_CONFIG_STR91X_TIMER_HZ
#else
#  define GYROS_CONFIG_HZ                      1000
#endif
#endif

#include <gyros/arch/arm/defconfig.h>
#include <gyros/private/defconfig.h>

#endif
