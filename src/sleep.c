#include <gyros/sleep.h>

#include <gyros/task.h>

#include "private.h"

static struct gyros_list_node s_sleeping = { &s_sleeping, &s_sleeping };

static void
add_task_to_sleeping(gyros_task_t *task)
{
    struct gyros_list_node *i;

    GYROS_LIST_FOR_EACH(i, &s_sleeping)
    {
        if ((long)(task->wakeup - TASK(i)->wakeup) < 0)
        {
            gyros_list_insert(&task->main_list, i->prev, i);
            return;
        }
    }

    gyros_list_add_last(&task->main_list, &s_sleeping);
}

void
gyros__wake_sleeping_tasks(void)
{
    unsigned long now = gyros__ticks;
    struct gyros_list_node *i;

    while (!gyros_list_empty(&s_sleeping) &&
           (long)(now - TASK(s_sleeping.next)->wakeup) >= 0)
    {
        i = s_sleeping.next;
        gyros_list_remove(i);
        gyros_list_remove(&TASK(i)->cond_list);
        gyros__add_task_to_running(TASK(i));
    }
}

void
gyros_sleep(unsigned long ticks)
{
    unsigned long flags;

    if (ticks == 0)
        return;

    flags = gyros_interrupt_disable();
    gyros__current_task->wakeup = gyros__ticks + ticks + 1;
    gyros_list_remove(&gyros__current_task->main_list);
    add_task_to_sleeping(gyros__current_task);
    gyros_interrupt_restore(flags);

    gyros__reschedule();
}

void
gyros_sleep_until(unsigned long ticks)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__current_task->wakeup = ticks;
    gyros_list_remove(&gyros__current_task->main_list);
    add_task_to_sleeping(gyros__current_task);
    gyros_interrupt_restore(flags);

    gyros__reschedule();
}
