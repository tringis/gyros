#include <gyros/mutex.h>

#include <stdlib.h>

#include "private.h"

#define SEM_TASK(t) GYROS_LIST_ENTRY(t, gyros_task_t, cond_list)

void
gyros_mutex_init(gyros_mutex_t *m)
{
    m->owner = NULL;
    GYROS_LIST_NODE_INIT(&m->task_list);
}

int
gyros_mutex_trylock(gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

    if (m->owner)
    {
        gyros_interrupt_restore(flags);
        return 0;
    }

    m->owner = gyros__current_task;
    m->owner_priority = gyros__current_task->priority;
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_mutex_lock(gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

    while (m->owner)
    {
        gyros__task_move(gyros__current_task, &m->task_list);
        /* Implement priority inheritance to prevent priority
         * inversion. */
        if (m->owner->priority < gyros__current_task->priority)
            m->owner->priority = gyros__current_task->priority;
        gyros__reschedule();
    }

    m->owner = gyros__current_task;
    m->owner_priority = gyros__current_task->priority;
    gyros_interrupt_restore(flags);
}

void
gyros__mutex_unlock(gyros_mutex_t *m, int reschedule)
{
    unsigned long flags = gyros_interrupt_disable();

    m->owner = NULL;
    gyros__current_task->priority = m->owner_priority;
    if (!gyros_list_empty(&m->task_list))
    {
        gyros__task_wake(TASK(m->task_list.next));
        if (reschedule)
            gyros__cond_reschedule();
    }
    gyros_interrupt_restore(flags);
}

void
gyros_mutex_unlock(gyros_mutex_t *m)
{
    gyros__mutex_unlock(m, 1);
}
