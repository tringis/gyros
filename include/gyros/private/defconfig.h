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
#ifndef INCLUDED__gyros_private_defconfig_h__200901021029
#define INCLUDED__gyros_private_defconfig_h__200901021029

#ifndef GYROS_CONFIG_DEBUG
#define GYROS_CONFIG_DEBUG 1
#endif

#ifndef GYROS_CONFIG_DYNTICK
#error GYROS_CONFIG_DYNTICK not defined by target defconfig.h
#endif

#ifndef GYROS_CONFIG_CUSTOM_IDLE_LOOP
#define GYROS_CONFIG_CUSTOM_IDLE_LOOP 0
#endif

#ifndef GYROS_CONFIG_ITERATE
#define GYROS_CONFIG_ITERATE 1
#endif

#ifndef GYROS_CONFIG_STACK_USED
#define GYROS_CONFIG_STACK_USED 1
#endif

#ifndef GYROS_CONFIG_TIME_TYPE
#define GYROS_CONFIG_TIME_TYPE long long
#endif

#ifndef GYROS_CONFIG_TIMER
#define GYROS_CONFIG_TIMER 1
#endif

#ifndef GYROS_CONFIG_CONTEXT_HOOK
#define GYROS_CONFIG_CONTEXT_HOOK 0
#endif

#ifndef GYROS_CONFIG_IRQ_HOOK
#define GYROS_CONFIG_IRQ_HOOK 0
#endif

#ifndef GYROS_CONFIG_TRACE
#define GYROS_CONFIG_TRACE 0
#endif

#endif
