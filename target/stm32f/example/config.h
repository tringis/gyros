/**************************************************************************
 * Copyright (c) 2002-2011, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDED__gyros_stm32f_config_h__201004131716
#define INCLUDED__gyros_stm32f_config_h__201004131716

/*---------------------------------------------------------------------*
 * GyrOS target specific configuration
 *---------------------------------------------------------------------*/
/* Core clock (HCLK) frequency */
#define GYROS_CONFIG_CORE_HZ                   72000000

/* Define one of the following to specify the STM32F family. */
#define GYROS_CONFIG_STM32F10x
/* #define GYROS_CONFIG_STM32F2xx */

/* AHB frequency (see RCC_CFGR[HPRE]) */
#define GYROS_CONFIG_STM32F_AHB_HZ             GYROS_CONFIG_CORE_HZ

/* APB1 frequency (see RCC_CFGR[PPRE1]) */
#define GYROS_CONFIG_STM32F_APB1_HZ            (GYROS_CONFIG_STM32F_AHB_HZ / 2)

/* APB2 frequency (see RCC_CFGR[PPRE2]) */
#define GYROS_CONFIG_STM32F_APB2_HZ            (GYROS_CONFIG_STM32F_AHB_HZ / 1)

/*---------------------------------------------------------------------*
 * Application specific configuration  (see gyros/target/defconfig.h
 *                                      and gyros/private/defconfig.h)
 *---------------------------------------------------------------------*/
#define GYROS_CONFIG_DEBUG                     1
#define GYROS_CONFIG_TRACE                     1

#include <gyros/target/defconfig.h>

#endif
