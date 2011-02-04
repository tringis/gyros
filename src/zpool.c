#include <gyros/memory.h>
#include <gyros/zpool.h>

#include <stddef.h>

#include <gyros/interrupt.h>
#include <gyros/private/debug.h>

#include "private.h"

#define GYROS_ZPOOL_MAGIC                 ((unsigned)0x726eaa11)

union gyros__zpool_bh
{
    union gyros__zpool_bh *next;     /* When in free list */
    gyros_zpool_t *pool;             /* When allocated */
};

gyros_zpool_t*
gyros_zpool_init(void *mem, unsigned mem_size, unsigned block_size)
{
    gyros_zpool_t *pool = mem;
    unsigned real_blksz = sizeof(union gyros__zpool_bh) +
        ((block_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1));
    union gyros__zpool_bh *bh = (union gyros__zpool_bh *)(pool + 1);
    unsigned i;

    pool->free_func = gyros_zpool_free;
    GYROS_DEBUG_INFO_INIT(pool, GYROS_ZPOOL_MAGIC);
    pool->total_blocks = (mem_size - sizeof(gyros_zpool_t)) /
                         real_blksz;
    pool->free_blocks = pool->total_blocks;
    pool->free_list = bh;
    for (i = 0; i < pool->total_blocks - 1; ++i)
    {
        bh->next = (union gyros__zpool_bh*)((unsigned long)bh + real_blksz);
        bh = bh->next;
    }
    bh->next = NULL;

    return pool;
}

unsigned
gyros_zpool_get_size(gyros_zpool_t *pool)
{
#if GYROS_CONFIG_DEBUG
    if (pool->debug_info.magic != GYROS_ZPOOL_MAGIC)
        gyros__error("uninitialized pool in zpool_get_size", pool);
#endif

    return pool->total_blocks;
}


unsigned
gyros_zpool_get_free(gyros_zpool_t *pool)
{
#if GYROS_CONFIG_DEBUG
    if (pool->debug_info.magic != GYROS_ZPOOL_MAGIC)
        gyros__error("uninitialized pool in zpool_get_free", pool);
#endif

    return pool->free_blocks;
}

void*
gyros_try_zalloc(gyros_zpool_t *pool)
{
    unsigned long flags = gyros_interrupt_disable();
    union gyros__zpool_bh *bh = pool->free_list;

#if GYROS_CONFIG_DEBUG
    if (pool->debug_info.magic != GYROS_ZPOOL_MAGIC)
        gyros__error("uninitialized pool in try_zalloc", pool);
#endif

    if (GYROS_UNLIKELY(bh == NULL))
    {
        gyros_interrupt_restore(flags);
        return NULL;
    }

    pool->free_blocks--;
    pool->free_list = bh->next;
    bh->pool = pool;

    gyros_interrupt_restore(flags);

    return bh + 1;
}

void*
gyros_zalloc(gyros_zpool_t *pool)
{
    void *zone = gyros_try_zalloc(pool);

    if (zone == NULL)
        gyros__error("Out of memory in gyros_zalloc", pool);

    return zone;
}

void
gyros_zpool_free(void *addr)
{
    union gyros__zpool_bh *bh = (union gyros__zpool_bh*)addr - 1;
    gyros_zpool_t *pool = bh->pool;
    unsigned long flags;

    flags = gyros_interrupt_disable();
    bh->next = pool->free_list;
    pool->free_list = bh;
    pool->free_blocks++;
    gyros_interrupt_restore(flags);
}
