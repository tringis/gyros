#include <gyros/cond.h>

#include <gyros/sleep.h>

#include "private.h"

void
gyros_cond_init(gyros_cond_t *c)
{
    GYROS_INIT_LIST_NODE(&c->task_list);
}

void
gyros_cond_wait(gyros_cond_t *c, gyros_mutex_t *m)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__mutex_unlock(m, 0);
    gyros_list_remove(&gyros__current_task->main_list);
    gyros__add_task_to_sec_list(&c->task_list, gyros__current_task);
    gyros_interrupt_restore(flags);

    gyros__reschedule();

    gyros_mutex_lock(m);
}

void
gyros_cond_timedwait(gyros_cond_t *c, gyros_mutex_t *m, int timeout)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__mutex_unlock(m, 0);
    gyros_list_remove(&gyros__current_task->main_list);
    gyros__add_task_to_sec_list(&c->task_list, gyros__current_task);
    gyros_interrupt_restore(flags);

    gyros_sleep_until(timeout);

    gyros_mutex_lock(m);
}

void
gyros_cond_signal_one(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

    if (!gyros_list_empty(&c->task_list))
    {
        struct gyros_list_node *task = c->task_list.next;

        gyros_list_remove(&SEC_TASK(task)->main_list);
        gyros_list_remove(task);
        gyros__add_task_to_running(SEC_TASK(task));
        gyros__reschedule();
    }
    gyros_interrupt_restore(flags);
}

void
gyros_cond_signal_all(gyros_cond_t *c)
{
    unsigned long flags = gyros_interrupt_disable();

    if (!gyros_list_empty(&c->task_list))
    {
        while (!gyros_list_empty(&c->task_list))
        {
            struct gyros_list_node *task = c->task_list.next;

            gyros_list_remove(&SEC_TASK(task)->main_list);
            gyros_list_remove(task);
            gyros__add_task_to_running(SEC_TASK(task));
        }
        gyros__reschedule();
    }
    gyros_interrupt_restore(flags);
}
