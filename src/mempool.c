#include <gyros/mempool.h>

#include <stddef.h>
#include <stdio.h>

#include <gyros/interrupt.h>
#include <gyros/private/debug.h>

#include "private.h"

#define PTR_ADD(ptr, bytes)  ((void*)((unsigned long)(ptr) + (bytes)))

#define GYROS_MEMPOOL_MAGIC                 ((unsigned)0x726eaa11)

/** \brief Memory pool object.  */
typedef struct gyros__mempool_slot
{
    unsigned block_size; /** Block size in bytes. */
    unsigned used_blocks; /** Highest number of used blocks. */
    struct gyros_mempool *pool; /** Parent memory pool. */
    union gyros__mempool_bh *free_list; /**< \internal */
} slot_t;

typedef union gyros__mempool_bh
{
    union gyros__mempool_bh *next;     /* When in free list */
    slot_t *slot;        /* When allocated */
} bh_t;


gyros_mempool_t*
gyros_mempool_init(const unsigned *block_sizes,
                   unsigned num_block_sizes,
                   void *memory,
                   unsigned memory_size)
{
    gyros_mempool_t *pool = memory;
    unsigned i, ctl_size = sizeof(gyros_mempool_t) +
                           num_block_sizes * sizeof(slot_t);

    if (ctl_size > memory_size)
        return NULL;

    GYROS_DEBUG_INFO_INIT(pool, GYROS_MEMPOOL_MAGIC);
    pool->total_bytes = memory_size - ctl_size;
    pool->unused_bytes = memory_size - ctl_size;
    pool->next_bh = PTR_ADD(memory, ctl_size);
    pool->slots = PTR_ADD(memory, sizeof(gyros_mempool_t));
    pool->num_slots = num_block_sizes;

    for (i = 0; i < num_block_sizes; ++i)
    {
        pool->slots[i].block_size = (block_sizes[i] + sizeof(void*) - 1) &
            ~(sizeof(void*) - 1);
        pool->slots[i].used_blocks = 0;
        pool->slots[i].pool = pool;
        pool->slots[i].free_list = NULL;
    }

    return pool;
}

void*
gyros_try_alloc(gyros_mempool_t *pool, unsigned size)
{
    bh_t *bh = NULL;
    unsigned i;

#if GYROS_CONFIG_DEBUG
    if (pool->debug_info.magic != GYROS_MEMPOOL_MAGIC)
        gyros__error("uninitialized pool in gyros_try_alloc", pool);
#endif

    for (i = 0; i < pool->num_slots; ++i)
    {
        if (size <= pool->slots[i].block_size)
        {
            slot_t *slot = pool->slots + i;
            unsigned long flags = gyros_interrupt_disable();

            bh = slot->free_list;
            if (bh)
                slot->free_list = bh->next;
            else if (sizeof(bh_t) + slot->block_size <= pool->unused_bytes)
            {
                unsigned usage = sizeof(bh_t) + slot->block_size;
                bh = pool->next_bh;
                pool->next_bh = PTR_ADD(pool->next_bh, usage);
                pool->unused_bytes -= usage;
                slot->used_blocks++;
            }
            if (bh)
                bh->slot = slot;
            gyros_interrupt_restore(flags);
            break;
        }
    }

    return bh ? bh + 1 : NULL;
}

void*
gyros_alloc(gyros_mempool_t *pool, unsigned size)
{
    void *zone = gyros_try_alloc(pool, size);

    if (zone == NULL)
        gyros__error("Out of memory in gyros_alloc", pool);

    return zone;
}

void
gyros_free(void *addr)
{
    if (addr != NULL)
    {
        bh_t *bh = (bh_t*)addr - 1;
        slot_t *slot = bh->slot;
        unsigned long flags = gyros_interrupt_disable();

#if GYROS_CONFIG_DEBUG
        if (slot->pool->debug_info.magic != GYROS_MEMPOOL_MAGIC)
            gyros__error("uninitialized pool in gyros_free", slot->pool);
#endif

        bh->next = slot->free_list;
        slot->free_list = bh;
        gyros_interrupt_restore(flags);
    }
}
