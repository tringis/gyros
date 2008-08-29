#include <gyros/sem.h>

#include <limits.h>

#include "private.h"

void
gyros_sem_init(gyros_sem_t *s, int start_value)
{
    s->value = start_value;
    s->max_value = UINT_MAX;
    GYROS_LIST_NODE_INIT(&s->task_list);
}

void
gyros_sem_init_binary(gyros_sem_t *s)
{
    s->value = 0;
    s->max_value = 1;
    GYROS_LIST_NODE_INIT(&s->task_list);
}

void
gyros_sem_wait(gyros_sem_t *s)
{
    unsigned long flags = gyros_interrupt_disable();

    while (s->value == 0)
    {
        gyros_list_remove(&gyros__current_task->list);
        gyros__add_task_to_list(&s->task_list, gyros__current_task);
        gyros__reschedule();
    }
    s->value--;
    gyros_interrupt_restore(flags);
}

int
gyros_sem_timedwait(gyros_sem_t *s, int timeout)
{
    unsigned long flags = gyros_interrupt_disable();

    if (s->value == 0)
    {
        gyros_list_remove(&gyros__current_task->list);
        gyros__add_task_to_list(&s->task_list, gyros__current_task);
        gyros__task_set_timeout(timeout);
        gyros__reschedule();
        if (s->value == 0)
        {
            gyros_interrupt_restore(flags);
            return 0;
        }
    }
    s->value--;
    gyros_interrupt_restore(flags);

    return 1;
}

void
gyros_sem_signal(gyros_sem_t *s)
{
    unsigned long flags = gyros_interrupt_disable();

    if (s->value < s->max_value)
    {
        s->value++;
        if (!gyros_list_empty(&s->task_list))
        {
            gyros__task_wake(TASK(s->task_list.next));
            gyros__reschedule();
        }
    }
    gyros_interrupt_restore(flags);
}
