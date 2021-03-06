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
#ifndef INCLUDED__gyros_sleep_h__200812301241
#define INCLUDED__gyros_sleep_h__200812301241

/** \file
  * \brief Sleep functions.
  * \details Header file for \ref sleep_group.
  *
  * \defgroup sleep_group Sleeping
  *
  * Sleeping is used to put a task on hold for a specified mimimum
  * amount of time, giving tasks of lower priority a chance to run.
  *
  * @{
  */

#include <gyros/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Sleep until the absolute time @a timeout has been reached.
  *
  * \param timeout      Absolute time to sleep until.
  * \return             True if @a time has been reached, or
  *                     false if the sleep was aborted prematurely.
  */
bool gyros_sleep_until(gyros_abstime_t timeout);

/** Sleep at least @a microseconds microseconds.
  *
  * \param time         Time to sleep.
  */
static inline void gyros_sleep(gyros_reltime_t time)
{
    gyros_abstime_t timeout = gyros_time() + time + 1;
    while (!gyros_sleep_until(timeout))
        ;
}

/** Sleep at least @a microseconds microseconds.
  *
  * \param microseconds Minimum number of microseconds to sleep.
  */
static inline void gyros_sleep_us(int microseconds)
{
    gyros_sleep(gyros_us(microseconds));
}

/** Sleep at least @a milliseconds milliseconds.
  *
  * \param milliseconds Minimum number of milliseconds to sleep.
  */
static inline void gyros_sleep_ms(int milliseconds)
{
    gyros_sleep(gyros_ms(milliseconds));
}

/** Sleep at least @a seconds seconds.
  *
  * \param seconds      Minimum number of seconds to sleep.
  */
static inline void gyros_sleep_s(int seconds)
{
    gyros_sleep(gyros_s(seconds));
}

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
