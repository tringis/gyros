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
#ifndef INCLUDED__gyros_memory_h__200902072124
#define INCLUDED__gyros_memory_h__200902072124

/** \defgroup memory_group Memory management
  *
  * TBD
  */
/*@{*/

/** \file
 * \brief memory management.
  * \copydoc task_group
 */

/** Create a zone pool, which is a memory pool where all blocks (or
  * zones) have the same size.  Zone pools are interrupt safe, meaning
  * that zones can be allocated and freed from interrupts.
  *
  * \param pool         Address of memory area to be used for the
  *                     pool.
  * \param pool_size    Size in bytes of memory area to be used for
  *                     the pool.
  * \param zone_size    Size of each zone.
  */
void gyros_zpool_init(void *pool, unsigned pool_size, unsigned zone_size);

/** Get the total number zones in the zone pool.
  *
  * \return             Total number of zones.
  */
unsigned gyros_zpool_get_size(void *pool);

/** Get the number of unallocated zones in the zone pool.
  *
  * \return             Total number of unallocated (free) zones.
  */
unsigned gyros_zpool_get_free(void *pool);

/** Allocate a zone from the zone pool.
  *
  * \param pool         Address of zone pool.
  * \return             Address to zone, or @c NULL if there were
  *                     no free zones.
  */
void *gyros_zalloc(void *pool);

/** Free memory at @a addr.  Does nothing if @a addr is @c NULL.
  *
  * \param addr         Address of memory to be freed.
  */
void gyros_free(void *addr);

/*@}*/

#endif
