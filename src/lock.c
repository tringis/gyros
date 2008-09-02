#include "private.h"

void
gyros_lock(void)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__state.locked++;
    gyros_interrupt_restore(flags);
}

void
gyros_unlock(void)
{
    unsigned long flags = gyros_interrupt_disable();

    if (--gyros__state.locked == 0)
        gyros__cond_reschedule();
    gyros_interrupt_restore(flags);
}
