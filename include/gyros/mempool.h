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
#ifndef INCLUDED__gyros_mempool_h__201102072205
#define INCLUDED__gyros_mempool_h__201102072205

#include <gyros-config.h>
#include <gyros/private/debug.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \file
  * \brief Memory pool management.
  * \details Header file for \ref mempool_group.
  *
  * \defgroup mempool_group Memory pool
  *
  * The memory pool allocator is an efficient allocator for fixed size
  * blocks.  Because it is for fixed size blocks, it is not
  * susceptible to fragmentation.
  *
  * @{
  */

/** \brief Memory pool object. */
typedef struct gyros_mempool
{
    struct gyros_debug_info debug_info; /**< \internal */

    unsigned total_bytes; /**< \internal */
    unsigned unused_bytes; /**< \internal */
    union gyros__mempool_bh *next_bh; /**< \internal */
    struct gyros__mempool_slot *slots; /**< \internal */
    unsigned num_slots; /**< \internal */
} gyros_mempool_t;

/** Create a new memory pool.  Memory pools are interrupt safe,
  * meaning that blocks can both be allocated and freed from
  * interrupts.
  *
  * \param block_sizes      Array of block sizes in ascending order.
  * \param num_block_sizes  Number of block sizes in @a block_sizes.
  * \param memory           Address of memory area to be used for the
  *                         pool.
  * \param memory_size      Size in bytes of memory area to be used for
  *                         the pool.
  * \return                 Pointer to pool or @c NULL if the memory
  *                         area is too small.
  */
gyros_mempool_t *gyros_mempool_init(const unsigned *block_sizes,
                                    unsigned num_block_sizes,
                                    void *memory,
                                    unsigned memory_size);

/** Try to allocate from the memory pool.  May be called from
  * interrupt context.  Call gyros_free() to free the allocated block.
  *
  * \param pool         Memory pool.
  * \param size         Number of bytes to allocate.
  * \return             Address to the allocated block, or @c NULL
  *                     if out of memory.
  */
void *gyros_try_alloc(gyros_mempool_t *pool, unsigned size);

/** Allocate from the memory pool.  May be called from interrupt
  * context.  Call gyros_free() to free the allocated zone.  Causes a
  * fatal error if no free zones.
  *
  * \param pool         Memory pool.
  * \param size         Number of bytes to allocate.
  * \return             Address to zone.
  */
void *gyros_alloc(gyros_mempool_t *pool, unsigned size);

/** Free memory that was allocated using gyros_try_alloc() or
  * gyros_alloc().  Does nothing if @a addr is @c NULL.  May be called
  * from interrupt context.
  *
  * \param addr         Address of memory to be freed.
  */
void gyros_free(void *addr);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
