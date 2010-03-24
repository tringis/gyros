#include <gyros/memory.h>
#include <gyros/zpool.h>

#include <stddef.h>

#include <gyros/interrupt.h>
#include <gyros/private/debug.h>

#define BPU (8U * sizeof(unsigned)) /* BPU = Bits Per Unsigned */

struct gyros__zpool
{
    void (*free_func)(void *addr);

    unsigned block_size;
    unsigned num_blocks;
    unsigned free_blocks;
    unsigned first_block_offset;
    unsigned bitmap[0];
};

struct block
{
    unsigned index;
    gyros_zpool_t *pool;
};

static void gyros_zpool_free(void *addr);

gyros_zpool_t*
gyros_zpool_init(void *mem, unsigned mem_size, unsigned block_size)
{
    gyros_zpool_t *pool = mem;
    unsigned data_words = (mem_size - sizeof(*pool)) / sizeof(unsigned);
    unsigned block_words = (block_size + sizeof(struct block) +
                            sizeof(unsigned) - 1) / sizeof(unsigned);
    unsigned bitmap_bytes, i;

    pool->free_func = gyros_zpool_free;
    pool->block_size = block_words * sizeof(unsigned);
    pool->num_blocks = data_words * BPU / (block_words * BPU + 1);
    pool->free_blocks = pool->num_blocks;
    bitmap_bytes = (pool->num_blocks + 31) / 32 * 4;
    pool->first_block_offset = sizeof(gyros_zpool_t) + bitmap_bytes;

    for (i = 0; i < bitmap_bytes / sizeof(unsigned); ++i)
        pool->bitmap[i] = 0;

    return pool;
}

unsigned
gyros_zpool_get_size(gyros_zpool_t *pool)
{
    return pool->num_blocks;
}


unsigned
gyros_zpool_get_free(gyros_zpool_t *pool)
{
    return pool->free_blocks;
}

void*
gyros_try_zalloc(gyros_zpool_t *pool)
{
    struct block *bh;
    unsigned word = 0, index = 0, mask = 1;
    unsigned long flags = gyros_interrupt_disable();

    while (word < pool->num_blocks / BPU && pool->bitmap[word] == ~0U)
        ++word;

    index = word * BPU;
    while (index < pool->num_blocks && (pool->bitmap[word] & mask))
    {
        ++index;
        mask <<= 1;
    }

    if (index >= pool->num_blocks)
    {
        gyros_interrupt_restore(flags);
        return NULL;
    }

    pool->bitmap[word] |= mask;
    pool->free_blocks--;
    gyros_interrupt_restore(flags);

    bh = (struct block*)((char*)pool + pool->first_block_offset +
                         index * pool->block_size);
    bh->pool = pool;
    bh->index = index;

    return bh + 1;
}

void*
gyros_zalloc(gyros_zpool_t *pool)
{
    void *zone = gyros_try_zalloc(pool);

    if (zone == NULL)
        gyros_error("Out of memory in gyros_zalloc", pool);

    return zone;
}

static void
gyros_zpool_free(void *addr)
{
    struct block *bh = (struct block*)addr - 1;
    unsigned long flags;

    if (!addr)
        return;

    flags = gyros_interrupt_disable();
#if GYROS_CONFIG_DEBUG
    if ((bh->pool->bitmap[bh->index / BPU] & (1U << (bh->index % BPU))) == 0)
        gyros_error("double free in gyros_zpool_free", addr);
#endif
    bh->pool->bitmap[bh->index / BPU] &= ~(1U << (bh->index % BPU));
    bh->pool->free_blocks++;
    gyros_interrupt_restore(flags);
}
