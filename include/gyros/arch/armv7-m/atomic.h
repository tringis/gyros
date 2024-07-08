/**************************************************************************
 * Copyright (c) 2002-2010, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDE__gyros_armv7_m_atomic_h__201004161532
#define INCLUDE__gyros_armv7_m_atomic_h__201004161532

#include <gyros/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYROS_HAS_LDREX_STREX

GYROS_ALWAYS_INLINE void *gyros_ldrex_p(void *addr)
{
    void *result;
  
    __asm__ volatile(
        "ldrex   %0, [%1]"
        : "=&r" (result) : "r" (addr));

    return result;
}

GYROS_ALWAYS_INLINE int gyros_strex_p(void *addr, void *value)
{
    unsigned result;
  
    __asm__ volatile(
        "strex   %0, %2, [%1]"
        : "=&r" (result) : "r" (addr), "r" (value));

    return !result;
}

#ifdef __cplusplus
}
#endif

#endif
