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
#ifndef INCLUDED__gyros_zpool_h__200902072124
#define INCLUDED__gyros_zpool_h__200902072124

/** \file
  * \brief Zone pool allocator.
  * \details Header file for \ref zpool_group.
  *
  * \defgroup zpool_group Zone pool allocator
  * \ingroup memory_group
  *
  * The zone pool allocator is an efficient allocator for fixed size
  * blocks (or zones).  Because it is for fixed size blocks, it is not
  * susceptible to fragmentation.  Double frees are detected if
  * \ref GYROS_CONFIG_DEBUG is enabled.
  *
  * @{
  */

/** Calculate the pool size in bytes required to store an exact number
  * of zones.
  *
  * \param num_zones    Number of zones.
  * \param zone_size    Size of the zones.
  * \return             Required pool size in bytes.
  */
#define GYROS_ZPOOL_SIZE(num_zones, zone_size)                          \
    (sizeof(void*) +                                                    \
     (4 + ((num_zones) + 8 * sizeof(unsigned) - 1) /                    \
          (8 * sizeof(unsigned)) * sizeof(unsigned)) +                  \
     (num_zones) * (sizeof(unsigned) + sizeof(void*) +                  \
                    ((zone_size) + sizeof(unsigned) - 1) /              \
                    sizeof(unsigned) * sizeof(unsigned)))

/** \brief Zone pool object. */
typedef struct gyros__zpool gyros_zpool_t;

/** Create a zone pool, which is a memory pool where all blocks (or
  * zones) have the same size.  Zone pools are interrupt safe, meaning
  * that zones can be allocated and freed from interrupts.
  *
  * \param mem          Address of memory area to be used for the
  *                     pool.
  * \param mem_size     Size in bytes of memory area to be used for
  *                     the pool.
  * \param zone_size    Size of each zone.
  * \return             Pointer to pool.
  */
gyros_zpool_t *gyros_zpool_init(void *mem, unsigned mem_size,
                                unsigned zone_size);

/** Get the total number zones in the zone pool.
  *
  * \param pool         Zone pool.
  * \return             Total number of zones.
  */
unsigned gyros_zpool_get_size(gyros_zpool_t *pool);

/** Get the number of unallocated zones in the zone pool.
  *
  * \param pool         Zone pool.
  * \return             Total number of unallocated (free) zones.
  */
unsigned gyros_zpool_get_free(gyros_zpool_t *pool);

/** Try to allocate a zone from the zone pool.  May be called from
  * interrupt context.  Call gyros_free() to free the allocated zone.
  *
  * \param pool         Zone pool.
  * \return             Address to zone, or @c NULL if there were
  *                     no free zones.
  */
void *gyros_try_zalloc(gyros_zpool_t *pool);

/** Allocate a zone from the zone pool.  May be called from interrupt
  * context.  Call gyros_free() to free the allocated zone.  Causes a
  * fatal error if no free zones.
  *
  * \param pool         Zone pool.
  * \return             Address to zone.
  */
void *gyros_zalloc(gyros_zpool_t *pool);

/*@}*/

#endif
