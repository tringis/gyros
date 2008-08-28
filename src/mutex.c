#include <gyros/mutex.h>

#include <stdlib.h>

#include "private.h"

#define SEM_TASK(t) GYROS_LIST_ENTRY(t, gyros_task_t, cond_list)

void
gyros_mutex_init(gyros_mutex_t *m)
{
    m->owner = NULL;
    GYROS_INIT_LIST_NODE(&m->task_list);
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
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_mutex_lock(gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

    while (m->owner)
    {
        gyros_list_remove(&gyros__current_task->main_list);
        gyros__add_task_to_main_list(&m->task_list, gyros__current_task);
        gyros__reschedule();
    }

    m->owner = gyros__current_task;
    gyros_interrupt_restore(flags);
}

void
gyros__mutex_unlock(gyros_mutex_t *m, int reschedule)
{
    unsigned long flags = gyros_interrupt_disable();

    m->owner = NULL;
    if (!gyros_list_empty(&m->task_list))
    {
        struct gyros_list_node *task = m->task_list.next;

        gyros_list_remove(task);
        gyros__add_task_to_running(MAIN_TASK(task));
        if (reschedule)
            gyros__reschedule();
    }
    gyros_interrupt_restore(flags);
}

void
gyros_mutex_unlock(gyros_mutex_t *m)
{
    gyros__mutex_unlock(m, 1);
}
