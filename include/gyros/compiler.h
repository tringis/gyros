/**************************************************************************
 * Copyright (c) 2002-2010, Tobias RingstrÃ¶m <tobias@ringis.se>
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
#ifndef INCLUDE__gyros_compiler_h__201004161629
#define INCLUDE__gyros_compiler_h__201004161629

/** \file
  * \brief Compiler support.
  * \details Header file for \ref compiler_group.
  *
  * \defgroup compiler_group Compiler support
  *
  * This group contains functions that allow compatible use of
  * compiler specific functionality.
  *
  * @{
  */

#ifdef __GNUC__
#define GYROS_LIKELY(x)       __builtin_expect((x),1)
#define GYROS_UNLIKELY(x)     __builtin_expect((x),0)
#define GYROS_ALWAYS_INLINE   static inline __attribute__((__always_inline__))
#else
/** Assist the compiler's optimizer by hinting that the expression @a
  * x is likely to be true.  Should be used very sparingly. */
#define GYROS_LIKELY(x)       (x)
/** Assist the compiler's optimizer by hinting that the expression @a
  * x is unlikely to be true.  Should be used very sparingly. */
#define GYROS_UNLIKELY(x)     (x)
/** Flag that the function should always be inlined. */
#define GYROS_ALWAYS_INLINE   static inline
#endif

/*@}*/

#endif
