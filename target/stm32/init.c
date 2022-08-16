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
#include <gyros/interrupt.h>
#include <gyros/private/target.h>
#include <gyros/arch/armv7-m/target.h>

//#include "../../arch/armv7-m/nvic.h"

#if defined(GYROS_CONFIG_STM32F1xx)
#   include <stm32f1xx.h>
#elif defined(GYROS_CONFIG_STM32F2xx)
#   include <stm32f2xx.h>
#elif defined(GYROS_CONFIG_STM32F4xx)
#   include <stm32f4xx.h>
#elif defined(GYROS_CONFIG_STM32F7xx)
#   include <stm32f7xx.h>
#elif defined(GYROS_CONFIG_STM32H7xx)
#   include <stm32h7xx.h>
#else
#   error "No GYROS_CONFIG_STM32xxx defined."
#endif

#if GYROS_CONFIG_DYNTICK

#if GYROS_CONFIG_STM32_TIMER == 1
#   define TIMx                           TIM1
#   define TIMER_IRQ                      TIM1_CC_IRQn
#   define TIMER_ISR                      TIM1_CC_IRQHandler
#elif GYROS_CONFIG_STM32_TIMER == 2
#   define TIMx                           TIM2
#   define TIMER_IRQ                      TIM2_IRQn
#   define TIMER_ISR                      TIM2_IRQHandler
#elif GYROS_CONFIG_STM32_TIMER == 3
#   define TIMx                           TIM3
#   define TIMER_IRQ                      TIM3_IRQn
#   define TIMER_ISR                      TIM3_IRQHandler
#elif GYROS_CONFIG_STM32_TIMER == 4
#   define TIMx                           TIM4
#   define TIMER_IRQ                      TIM4_IRQn
#   define TIMER_ISR                      TIM4_IRQHandler
#elif GYROS_CONFIG_STM32_TIMER == 5
#   define TIMx                           TIM5
#   define TIMER_IRQ                      TIM5_IRQn
#   define TIMER_ISR                      TIM5_IRQHandler
#elif GYROS_CONFIG_STM32_TIMER == 8
#   define TIMx                           TIM8
#   define TIMER_IRQ                      TIM8_CC_IRQn
#   define TIMER_ISR                      TIM8_CC_IRQHandler
#else
#   error Unsupported GYROS_CONFIG_STM32_TIMER value
#endif

#if defined(GYROS_CONFIG_STM32F1xx)
#   define TIMER_BITS                     16
#elif GYROS_CONFIG_STM32_TIMER ==  2 || \
      GYROS_CONFIG_STM32_TIMER ==  5 || \
      GYROS_CONFIG_STM32_TIMER == 23 || \
      GYROS_CONFIG_STM32_TIMER == 24
#   define TIMER_BITS                     32
#else
#   define TIMER_BITS                     16
#endif

#if TIMER_BITS == 16
#   define TIMER_T                        short
#   define MAX_PERIOD                     0x8000
#elif TIMER_BITS == 32
#   define TIMER_T                        int
#   define MAX_PERIOD                     0x80000000
#endif

static gyros_abstime_t s_time_hi;
static unsigned TIMER_T s_last_time_lo;

void
TIMER_ISR(void)
{
    TIMx->SR = ~TIM_SR_CC1IF; /* Clear match interrupt */
    gyros__tick(gyros_time());
}

void
gyros__dyntick_suspend(void)
{
    /* We can't suspend the tick, because it would make gyros_time()
     * fail, so we set it to the maximum period. */
    TIMx->CCR1 = s_last_time_lo + MAX_PERIOD;
}

void
gyros__dyntick_set(gyros_abstime_t now, gyros_abstime_t next_timeout)
{
    gyros_reltime_t dt = next_timeout - now;

    if (dt >= MAX_PERIOD)
        TIMx->CCR1 = s_last_time_lo + MAX_PERIOD;
    else
    {
        TIMx->CCR1 = next_timeout;

        /* If dt is small enough to be a risk, we check if the time is
         * already past, and if so just pend the timer interrupt. */
        if (dt < 0x2000 &&
            (signed TIMER_T)((unsigned TIMER_T)next_timeout - TIMx->CNT) <= 0)
        {
            gyros_target_pend_irq(TIMER_IRQ);
        }
    }
}

gyros_abstime_t
gyros_time(void)
{
    unsigned long flags = gyros_interrupt_disable();
    unsigned TIMER_T time_lo = TIMx->CNT;
    gyros_abstime_t time_hi;

    if (time_lo < s_last_time_lo)
        ++s_time_hi;
    s_last_time_lo = time_lo;
    time_hi = s_time_hi;

    gyros_interrupt_restore(flags);

    return (time_hi << TIMER_BITS) | time_lo;
}
#endif

void
gyros__target_init(void)
{
    gyros__arch_init();

#if GYROS_CONFIG_DYNTICK

    TIMx->PSC = GYROS_CONFIG_STM32_TIMER_HZ / GYROS_CONFIG_HZ - 1;
#if TIMER_BITS == 16
    TIMx->ARR = 0xffff;
#else
    TIMx->ARR = 0xffffffff;
#endif
    TIMx->EGR |= TIM_EGR_UG; /* Generate update event to load PSC and ARR */

    gyros_target_enable_irq(TIMER_IRQ, GYROS_CONFIG_SYSTICK_PRIORITY);

    TIMx->DIER = TIM_DIER_CC1IE; /* Enable CC1 match interrupt */
    TIMx->CR1 |= TIM_CR1_CEN; /* Enable timer */

    gyros__dyntick_suspend();

#endif /* GYROS_CONFIG_DYNTICK */
}
