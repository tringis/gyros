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
#include <gyros/interrupt.h>
#include <gyros/private/target.h>
#include <gyros/arch/armv7-m/target.h>

#include "../../arch/armv7-m/nvic.h"

#if GYROS_CONFIG_DYNTICK
#  if GYROS_CONFIG_STM32_TIMER == 2
#    define TIMER_APB1_MASK      (1U <<  0)
#    define TIMER_DBG_CR_MASK    (1U <<  11)
#    define TIMER_IRQ            28
#    define TIMER_ISR            TIM2_IRQHandler
#    define TIMER_ADDR           0x40000000
#  elif GYROS_CONFIG_STM32_TIMER == 3
#    define TIMER_APB1_MASK      (1U <<  1)
#    define TIMER_DBG_CR_MASK    (1U <<  12)
#    define TIMER_IRQ            29
#    define TIMER_ISR            TIM3_IRQHandler
#    define TIMER_ADDR           0x40000400
#  elif GYROS_CONFIG_STM32_TIMER == 4
#    define TIMER_APB1_MASK      (1U <<  2)
#    define TIMER_DBG_CR_MASK    (1U <<  13)
#    define TIMER_IRQ            30
#    define TIMER_ISR            TIM4_IRQHandler
#    define TIMER_ADDR           0x40000800
#  elif GYROS_CONFIG_STM32_TIMER == 5
#    define TIMER_APB1_MASK      (1U <<  3)
#    define TIMER_DBG_CR_MASK    (1U <<  18)
#    define TIMER_IRQ            50
#    define TIMER_ISR            TIM5_IRQHandler
#    define TIMER_ADDR           0x40000c00
#  else
#    error Invalid GYROS_CONFIG_STM32_TIMER value
#  endif

#define MAX_PERIOD           0x8000

#define REG16(addr)          (*(volatile unsigned short*)(addr))
#define BIT16(n)             ((unsigned short)1 << (n))
#define REG32(addr)          (*(volatile unsigned long*)(addr))

#define DBGMCU_CR            REG32(0xE0042004)

#define RCC_REG(offset)      REG32(0x40021000 + (offset))

#define RCC_APB1RSTR         RCC_REG(0x10)
#define RCC_APB1ENR          RCC_REG(0x1c)

#define TIMER_REG(offset)    REG16(TIMER_ADDR + (offset))

#define TIMx_CR1             TIMER_REG(0x00)
#define TIMx_CR1_CEN         BIT16(0)
#define TIMx_CR2             TIMER_REG(0x04)
#define TIMx_CSMCR           TIMER_REG(0x08)
#define TIMx_DIER            TIMER_REG(0x0c)
#define TIMx_DIER_UIE        BIT16(0)
#define TIMx_DIER_CC1IE      BIT16(1)
#define TIMx_DIER_CC2IE      BIT16(2)
#define TIMx_DIER_CC3IE      BIT16(3)
#define TIMx_DIER_CC4IE      BIT16(4)
#define TIMx_SR              TIMER_REG(0x10)
#define TIMx_SR_UIF          BIT16(0)
#define TIMx_SR_CC1IF        BIT16(1)
#define TIMx_SR_CC2IF        BIT16(2)
#define TIMx_SR_CC3IF        BIT16(3)
#define TIMx_SR_CC4IF        BIT16(4)
#define TIMx_EGR             TIMER_REG(0x14)
#define TIMx_EGR_UG          BIT16(0)
#define TIMx_CCMR1           TIMER_REG(0x18)
#define TIMx_CCMR2           TIMER_REG(0x1c)
#define TIMx_CCER            TIMER_REG(0x20)
#define TIMx_CNT             TIMER_REG(0x24)
#define TIMx_PSC             TIMER_REG(0x28)
#define TIMx_ARR             TIMER_REG(0x2c)
#define TIMx_CCR1            TIMER_REG(0x34)
#define TIMx_CCR2            TIMER_REG(0x38)
#define TIMx_CCR3            TIMER_REG(0x3c)
#define TIMx_CCR4            TIMER_REG(0x40)
#define TIMx_DCR             TIMER_REG(0x48)
#define TIMx_DMAR            TIMER_REG(0x4c)

static gyros_abstime_t s_time_hi;
static unsigned short s_last_time_lo;

void
TIMER_ISR(void)
{
    TIMx_SR = ~TIMx_SR_CC1IF; /* Clear match interrupt */
    gyros__tick(gyros_time());
}

void
gyros__dyntick_suspend(void)
{
    /* We can't suspend the tick, because it would make gyros_time()
     * fail, so we set it to the maximum period. */
    TIMx_CCR1 = s_last_time_lo + MAX_PERIOD;
}

void
gyros__dyntick_set(gyros_abstime_t now, gyros_abstime_t next_timeout)
{
    gyros_reltime_t dt = next_timeout - now;

    if (dt >= MAX_PERIOD)
        TIMx_CCR1 = s_last_time_lo + MAX_PERIOD;
    else
    {
        TIMx_CCR1 = next_timeout;

        /* If dt is small enough to be a risk, we check if the time is
         * already past, and if so just pend the timer interrupt. */
        if (dt < MAX_PERIOD / 4 &&
            (short)((unsigned short)next_timeout - TIMx_CNT) <= 0)
        {
            gyros_target_pend_irq(TIMER_IRQ);
        }
    }
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    unsigned short time_lo = TIMx_CNT;
    gyros_abstime_t time_hi;

    if (time_lo < s_last_time_lo)
        ++s_time_hi;
    s_last_time_lo = time_lo;
    time_hi = s_time_hi;

    gyros_interrupt_restore(flags);

    return (time_hi << 16) | time_lo;
}
#endif

void
gyros__target_init(void)
{
    gyros__arch_init();

#if GYROS_CONFIG_DYNTICK
    RCC_APB1ENR |= TIMER_APB1_MASK;
    RCC_APB1RSTR |= TIMER_APB1_MASK;
    RCC_APB1RSTR &= ~TIMER_APB1_MASK;

    TIMx_PSC = GYROS_CONFIG_CORE_HZ / GYROS_CONFIG_STM32_TIMER_HZ - 1;
    TIMx_ARR = 0xffff;
    TIMx_EGR |= TIMx_EGR_UG; /* Generate update even to load PSC and ARR */

    DBGMCU_CR |= TIMER_DBG_CR_MASK; /* Stop timer when CPU halted. */

    gyros_target_enable_irq(TIMER_IRQ, GYROS_CONFIG_SYSTICK_PRIORITY);

    TIMx_DIER = TIMx_DIER_CC1IE; /* Enable CC1 match interrupt */
    TIMx_CR1 |= TIMx_CR1_CEN; /* Enable timer */

    gyros__dyntick_suspend();
#endif
}
