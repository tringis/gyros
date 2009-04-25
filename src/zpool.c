#include <gyros/memory.h>

#include <stddef.h>

#include <gyros/interrupt.h>
#include <gyros/private/debug.h>

#define BPU (8U * sizeof(unsigned)) /* BPU = Bits Per Unsigned */

struct zpool
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
    struct zpool *pool;
};

static void gyros_zpool_free(void *addr);

void
gyros_zpool_init(void *pool, unsigned pool_size, unsigned block_size)
{
    struct zpool *ep = pool;
    unsigned data_words = (pool_size - sizeof(*ep)) / sizeof(unsigned);
    unsigned block_words = (block_size + sizeof(struct block) +
                            sizeof(unsigned) - 1) / sizeof(unsigned);
    unsigned bitmap_bytes, i;

    ep->free_func = gyros_zpool_free;
    ep->block_size = block_words * sizeof(unsigned);
    ep->num_blocks = data_words * BPU / (block_words * BPU + 1);
    ep->free_blocks = ep->num_blocks;
    bitmap_bytes = (ep->num_blocks + 31) / 32 * 4;
    ep->first_block_offset = sizeof(struct zpool) + bitmap_bytes;

    for (i = 0; i < bitmap_bytes / sizeof(unsigned); ++i)
        ep->bitmap[i] = 0;
}

unsigned
gyros_zpool_get_size(void *pool)
{
    struct zpool *ep = pool;

    return ep->num_blocks;
}


unsigned
gyros_zpool_get_free(void *pool)
{
    struct zpool *ep = pool;

    return ep->free_blocks;
}

void*
gyros_zalloc(void *pool)
{
    struct zpool *ep = pool;
    struct block *bh;
    unsigned word = 0, index = 0, mask = 1;
    unsigned long flags = gyros_interrupt_disable();

    while (word < ep->num_blocks / BPU && ep->bitmap[word] == ~0U)
        ++word;

    index = word * BPU;
    while (index < ep->num_blocks && (ep->bitmap[word] & mask))
    {
        ++index;
        mask <<= 1;
    }

    if (index >= ep->num_blocks)
    {
        gyros_interrupt_restore(flags);
        return NULL;
    }

    ep->bitmap[word] |= mask;
    ep->free_blocks--;
    gyros_interrupt_restore(flags);

    bh = (struct block*)((char*)ep + ep->first_block_offset +
                         index * ep->block_size);
    bh->pool = ep;
    bh->index = index;

    return bh + 1;
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
        gyros_error("double free in gyros_zpool_free");
#endif
    bh->pool->bitmap[bh->index / BPU] &= ~(1U << (bh->index % BPU));
    bh->pool->free_blocks++;
    gyros_interrupt_restore(flags);
}
