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
#ifndef INCLUDED__gyros_arch_armv7_m_nvic_h__201002071208
#define INCLUDED__gyros_arch_armv7_m_nvic_h__201002071208

#define NVIC_REG32(offset)         (*(volatile unsigned long*)(0xe000e000 + \
                                                               (offset)))

#define NVIC_SYSTICK_CTLST         NVIC_REG32(0x010)
#define NVIC_SYSTICK_RELOAD        NVIC_REG32(0x014)
#define NVIC_SYSTICK_CURRENT       NVIC_REG32(0x018)
#define NVIC_SYSTICK_CALIB         NVIC_REG32(0x01c)

#define NVIC__IRQ_BITCMD(offset, n)  \
    do { NVIC_REG32(offset + ((n) >> 5)) = 1U << ((n) & 31); } while (0)

#define NVIC_IRQ_SET_ENABLE(n)     NVIC__IRQ_BITCMD(0x100, (n))
#define NVIC_IRQ_CLEAR_ENABLE(n)   NVIC__IRQ_BITCMD(0x180, (n))
#define NVIC_IRQ_SET_PENDING(n)    NVIC__IRQ_BITCMD(0x200, (n))
#define NVIC_IRQ_CLEAR_PENDING(n)  NVIC__IRQ_BITCMD(0x280, (n))
#define NVIC_IRQ_ACTIVE(n)         NVIC__IRQ_BITCMD(0x300, (n))

#define NVIC_IRQ_PRIORITY(n)       NVIC_REG32(0x400 + (n))

#define NVIC_IRQ_CSR               NVIC_REG32(0xd04)
#define NVIC_VTABOFFSET            NVIC_REG32(0xd08)

#endif
