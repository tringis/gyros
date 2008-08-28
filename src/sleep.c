#include <gyros/sleep.h>

#include <gyros/task.h>

#include "private.h"

#define TIMEOUT_TASK(t) GYROS_LIST_CONTAINER(t, gyros_task_t, timeout_list)

static struct gyros_list_node s_sleeping = { &s_sleeping, &s_sleeping };

void
gyros__task_set_timeout(unsigned long timeout)
{
    struct gyros_list_node *i;

    for (i = s_sleeping.next; i != &s_sleeping; i = i->next)
    {
        if ((long)(gyros__current_task->wakeup - TIMEOUT_TASK(i)->wakeup) < 0)
            break;
    }
    gyros_list_insert_before(&gyros__current_task->timeout_list, i);
    gyros__current_task->wakeup = timeout;
}

void
gyros__wake_sleeping_tasks(void)
{
    unsigned long now = gyros__ticks;

    while (!gyros_list_empty(&s_sleeping) &&
           (long)(now - TIMEOUT_TASK(s_sleeping.next)->wakeup) >= 0)
    {
        gyros__task_wake(TIMEOUT_TASK(s_sleeping.next));
    }
}

void
gyros_sleep(unsigned long ticks)
{
    unsigned long flags;

    if (ticks == 0)
        return;

    flags = gyros_interrupt_disable();
    gyros_list_remove(&gyros__current_task->list);
    gyros__task_set_timeout(gyros__ticks + ticks + 1);
    gyros_interrupt_restore(flags);
    gyros__reschedule();
}

void
gyros_sleep_until(unsigned long timeout)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros_list_remove(&gyros__current_task->list);
    gyros__task_set_timeout(timeout);
    gyros_interrupt_restore(flags);
    gyros__reschedule();
}
