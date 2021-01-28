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
#ifndef INCLUDE__gyros_time_h__200808291459
#define INCLUDE__gyros_time_h__200808291459

/** \file
  * \brief Time management.
  * \details Header file for \ref time_group.
  *
  * \defgroup time_group Time management
  *
  * There are two distinct data types for expressing time in GyrOS:
  *
  * \li \ref gyros_abstime_t is used for absolute time, meaning a
  *     fixed time in the past or in the future.  It expresses
  *     <em>when</em>, not <em>for how long</em>.
  * \li \ref gyros_reltime_t is used for relative time, meaning a
  *     fixed time in the past or in the future.  It expresses <em>for
  *     how long</em>, not <em>when</em>.
  *
  * Both these types are scalar types, but the exact type is
  * configurable using \ref GYROS_CONFIG_TIME_TYPE.
  *
  * All GyrOS functions that has a timeout, e.g.
  * gyros_sem_wait_until(), takes an absolute time as argument, which
  * means that the timeout is normally calculated by adding a relative
  * time to the return value of gyros_time().
  *
  * @{
  */

#include <stdbool.h>

#include <gyros/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Absolute monotonically increasing time in target specific
  * units. */
typedef unsigned GYROS_CONFIG_TIME_TYPE gyros_abstime_t;

/** Relative time in target specific units. */
typedef signed GYROS_CONFIG_TIME_TYPE gyros_reltime_t;

/** Convert @a microseconds to gyros_reltime_t.  May be called from
  * interrupt context. */
static inline gyros_reltime_t gyros_us(long microseconds)
{
#if defined(GYROS_CONFIG_US_TO_TICKS)
    return GYROS_CONFIG_US_TO_TICKS(microseconds);
#elif GYROS_CONFIG_HZ / 1000000 * 1000000 == GYROS_CONFIG_HZ
    return microseconds * (gyros_reltime_t)(GYROS_CONFIG_HZ / 1000000);
#elif GYROS_CONFIG_HZ / 100000 * 100000 == GYROS_CONFIG_HZ
    return microseconds *
        ((gyros_reltime_t)(GYROS_CONFIG_HZ / 100000) + 9) / 10;
#elif GYROS_CONFIG_HZ / 10000 * 10000 == GYROS_CONFIG_HZ
    return microseconds *
        ((gyros_reltime_t)(GYROS_CONFIG_HZ / 10000) + 99) / 100;
#else
    return microseconds *
        ((gyros_reltime_t)((GYROS_CONFIG_HZ + 999) / 1000) + 999) / 1000;
#endif
}

/** Convert @a milliseconds to gyros_reltime_t.  May be called from
  * interrupt context. */
static inline gyros_reltime_t gyros_ms(long milliseconds)
{
#if defined(GYROS_CONFIG_MS_TO_TICKS)
    return GYROS_CONFIG_MS_TO_TICKS(milliseconds);
#else
    return milliseconds * (gyros_abstime_t)((GYROS_CONFIG_HZ + 999) / 1000);
#endif
}

/** Convert @a seconds to gyros_reltime_t.  May be called from
  * interrupt context. */
static inline gyros_reltime_t gyros_s(long seconds)
{
    return seconds * (gyros_abstime_t)GYROS_CONFIG_HZ;
}

/** Convert @a gyros_reltime_t to microseconds.  May be called from
  * interrupt context. */
static inline long gyros_time_to_us(gyros_reltime_t time)
{
#if defined(GYROS_CONFIG_TICKS_TO_US)
    return GYROS_CONFIG_TICKS_TO_US(time);
#elif GYROS_CONFIG_HZ / 1000000 * 1000000 == GYROS_CONFIG_HZ
    return time / (gyros_reltime_t)(GYROS_CONFIG_HZ / 1000000);
#elif GYROS_CONFIG_HZ / 100000 * 100000 == GYROS_CONFIG_HZ
    return time * (gyros_reltime_t)10 / (GYROS_CONFIG_HZ / 100000);
#elif GYROS_CONFIG_HZ / 10000 * 10000 == GYROS_CONFIG_HZ
    return time * (gyros_reltime_t)100 / (GYROS_CONFIG_HZ / 10000);
#elif GYROS_CONFIG_HZ / 1000 * 1000 == GYROS_CONFIG_HZ
    return time * (gyros_reltime_t)1000 / (GYROS_CONFIG_HZ / 1000);
#elif GYROS_CONFIG_HZ / 100 * 100 == GYROS_CONFIG_HZ
    return time * (gyros_reltime_t)10000 / (GYROS_CONFIG_HZ / 100);
#elif GYROS_CONFIG_HZ / 100 * 100 == GYROS_CONFIG_HZ
    return time * (gyros_reltime_t)100000 / (GYROS_CONFIG_HZ / 10);
#else
    return time * (gyros_reltime_t)1000000 / GYROS_CONFIG_HZ;
#endif
}

/** Convert @a gyros_reltime_t to milliseconds.  May be called from
  * interrupt context. */
static inline long gyros_time_to_ms(gyros_reltime_t time)
{
#if defined(GYROS_CONFIG_TICKS_TO_MS)
    return GYROS_CONFIG_TICKS_TO_MS(time);
#elif GYROS_CONFIG_HZ / 1000 * 1000 == GYROS_CONFIG_HZ
    return time / (GYROS_CONFIG_HZ / 1000);
#elif GYROS_CONFIG_HZ / 100 * 100 == GYROS_CONFIG_HZ
    return time * (gyros_reltime_t)10 / (GYROS_CONFIG_HZ / 100);
#elif GYROS_CONFIG_HZ / 100 * 100 == GYROS_CONFIG_HZ
    return time * (gyros_reltime_t)100 / (GYROS_CONFIG_HZ / 10);
#else
    return time * (gyros_reltime_t)1000 / GYROS_CONFIG_HZ;
#endif
}

/** Convert @a gyros_reltime_t to seconds.  May be called from
  * interrupt context. */
static inline long gyros_time_to_s(gyros_reltime_t time)
{
    return time / GYROS_CONFIG_HZ;
}

/** Return current absolute time.  The time is monotonically
  * increasing.  Use time_us(), time_ms(), time_s(),
  * gyros_time_to_us(), gyros_time_to_us() and gyros_time_to_s() to
  * convert to and from real time.  May be called from interrupt
  * context.
  *
  * \return             Current absolute time.
  */
gyros_abstime_t gyros_time(void);

/** Compare @a time1 and @a time2.  May be called from interrupt
  * context.
  *
  * \param time1        First time to compare.
  * \param time2        Second time to compare.
  * \return             A negative value if @a time1 is less than
  *                     @a time2, zero if @a time1 is equal to
  *                     @a time2, or a positive value if @a time1
  *                     is greater than @a time2.
  */
static inline gyros_reltime_t gyros_time_compare(gyros_abstime_t time1,
                                                 gyros_abstime_t time2)
{
    return (gyros_reltime_t)(time1 - time2);
}

/** Helper function to test if @a time has been reached.  May be
  * called from interrupt context.
  *
  * \param time         Absolute time to test.
  * \return             True if @a time has been reached, else false.
  */
static inline bool gyros_time_reached(gyros_abstime_t time)
{
    return gyros_time_compare(time, gyros_time()) <= 0;
}

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
