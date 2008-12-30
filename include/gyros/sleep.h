/**************************************************************************
 * Copyright (c) 2002-2008, Tobias Ringström <tobias@ringis.se>
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

/** \file sleep.h
 * \brief Sleep functions.
 */

#include <gyros/time.h>

/** Sleep until the absolute time @a timeout has been reached.
  *
  * \param timeout      Absolute time to sleep until.
  * \return             Non-zero if @a time has been reached, or
  *                     zero if the sleep was aborted prematurely.
  */
int gyros_sleep_until(gyros_abstime_t timeout);

/** Sleep at least @a microseconds microseconds.
  *
  * \param microseconds Minimum number of microseconds to sleep.
  * \return             Non-zero if the sleep was complete, or
  *                     zero if it was aborted prematurely.
  */
static inline int gyros_sleep_us(int microseconds)
{
    return gyros_sleep_until(gyros_time() + gyros_us(microseconds) + 1);
}

/** Sleep at least @a milliseconds milliseconds.
  *
  * \param milliseconds Minimum number of milliseconds to sleep.
  * \return             Non-zero if the sleep was complete, or
  *                     zero if it was aborted prematurely.
  */
static inline int gyros_sleep_ms(int milliseconds)
{
    return gyros_sleep_until(gyros_time() + gyros_ms(milliseconds) + 1);
}

/** Sleep at least @a seconds seconds.
  *
  * \param seconds      Minimum number of seconds to sleep.
  * \return             Non-zero if the sleep was complete, or
  *                     zero if it was aborted prematurely.
  */
static inline int gyros_sleep_s(int seconds)
{
    return gyros_sleep_until(gyros_time() + gyros_s(seconds) + 1);
}

#endif
