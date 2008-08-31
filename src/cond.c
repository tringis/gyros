#include <gyros/cond.h>

#include "private.h"

void
gyros_cond_init(gyros_cond_t *c)
{
    GYROS_LIST_NODE_INIT(&c->task_list);
}

void
gyros_cond_wait(gyros_cond_t *c, gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__mutex_unlock(m, 0);
    gyros__task_move(gyros__current_task, &c->task_list);
    gyros_interrupt_restore(flags);

    gyros__reschedule();

    gyros_mutex_lock(m);
}

void
gyros_cond_timedwait(gyros_cond_t *c, gyros_mutex_t *m, int timeout)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__mutex_unlock(m, 0);
    gyros__task_move(gyros__current_task, &c->task_list);
    gyros__task_timeout(timeout);
    gyros_interrupt_restore(flags);

    gyros_mutex_lock(m);
}

void
gyros_cond_signal_one(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

    if (!gyros_list_empty(&c->task_list))
    {
        gyros__task_wake(TASK(c->task_list.next));
        gyros__cond_reschedule();
    }
    gyros_interrupt_restore(flags);
}

void
gyros_cond_signal_all(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

    if (!gyros_list_empty(&c->task_list))
    {
        /* Move the tasks in reverse order to preserve the order of
         * the tasks (of equal priority) in the list. */
        while (!gyros_list_empty(&c->task_list))
            gyros__task_wake(TASK(c->task_list.prev));
        gyros__cond_reschedule();
    }
    gyros_interrupt_restore(flags);
}
