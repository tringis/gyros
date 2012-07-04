/**************************************************************************
 * Copyright (c) 2002-2012, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDED__gyros_ep93xx_h__200808312206
#define INCLUDED__gyros_ep93xx_h__200808312206

#define REG32(addr)   (*(volatile unsigned*)(addr))

typedef volatile unsigned short reg16;
typedef volatile unsigned reg32;

struct VIC_regs
{
    reg32 IRQStatus;
    reg32 FIQStatus;
    reg32 RawIntr;
    reg32 IntSelect;
    reg32 IntEnable;
    reg32 IntEnClear;
    reg32 SoftInt;
    reg32 SoftIntClear;
    reg32 Protection;
    reg32 unused0[3];
    reg32 CurVectAddr;
    reg32 DefVectAddr;
    reg32 unused1[50];
    reg32 VectAddr[16];
    reg32 unused2[48];
    reg32 VectCntl[16];
};

#define VIC(i)       ((struct VIC_regs*)(0x800B0000 + (i) * 0x10000))

#define VIC1         ((struct VIC_regs*)0x800B0000)
#define VIC2         ((struct VIC_regs*)0x800C0000)

#define TIMx_ENABLE  (1U << 7)
#define TIMx_MODE    (1U << 6)
#define TIMx_CLKSEL  (1U << 3)

#define TIM1_Load    REG32(0x80810000)
#define TIM1_Value   REG32(0x80810004)
#define TIM1_Cntl    REG32(0x80810008)
#define TIM1_Clear   REG32(0x8081000C)

#endif
