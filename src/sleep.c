#include <gyros/task.h>

#include "private.h"

#define TIMEOUT_TASK(t) GYROS_LIST_CONTAINER(t, gyros_task_t, timeout_list)

static struct gyros_list_node s_sleeping = { &s_sleeping, &s_sleeping };

int
gyros__task_timeout(unsigned long timeout)
{
    struct gyros_list_node *i;

    for (i = s_sleeping.next; i != &s_sleeping; i = i->next)
    {
        if ((long)(timeout - TIMEOUT_TASK(i)->timeout) < 0)
            break;
    }
    gyros_list_insert_before(&gyros__state.current->timeout_list, i);
    gyros__state.current->timeout = timeout;
    gyros__state.current->timed_out = 0;
    gyros__reschedule();

    return gyros__state.current->timed_out;
}

void
gyros__wake_sleeping_tasks(void)
{
    unsigned long now = gyros__ticks;

    while (!gyros_list_empty(&s_sleeping) &&
           (long)(now - TIMEOUT_TASK(s_sleeping.next)->timeout) >= 0)
    {
        gyros_task_t *task = TIMEOUT_TASK(s_sleeping.next);

        task->timed_out = 1;
        gyros__task_wake(task);
    }
}

int
gyros_sleep(unsigned long ticks)
{
    unsigned long flags;
    int timed_out;

    if (ticks == 0)
        return 1;

    flags = gyros_interrupt_disable();
    gyros_list_remove(&gyros__state.current->main_list);
    timed_out = gyros__task_timeout(gyros__ticks + ticks + 1);
    gyros_interrupt_restore(flags);

    return timed_out;
}

int
gyros_sleep_until(unsigned long timeout)
{
    unsigned long flags = gyros_interrupt_disable();
    int timed_out;

    gyros_list_remove(&gyros__state.current->main_list);
    timed_out = gyros__task_timeout(timeout);
    gyros_interrupt_restore(flags);

    return timed_out;
}
