#include <gyros/sem.h>

#include <gyros/sleep.h>

#include <limits.h>

#include "private.h"

void
gyros_sem_init(gyros_sem_t *s, int start_value)
{
    s->value = start_value;
    s->max_value = UINT_MAX;
    GYROS_INIT_LIST_NODE(&s->task_list);
}

void
gyros_sem_init_binary(gyros_sem_t *s)
{
    s->value = 0;
    s->max_value = 1;
    GYROS_INIT_LIST_NODE(&s->task_list);
}

void
gyros_sem_wait(gyros_sem_t *s)
{
    unsigned long flags = gyros_interrupt_disable();

    while (s->value == 0)
    {
        gyros_list_remove(&gyros__current_task->main_list);
        gyros__add_task_to_sec_list(&s->task_list, gyros__current_task);
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
        gyros__add_task_to_sec_list(&s->task_list, gyros__current_task);
        gyros_sleep_until(timeout);
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
            struct gyros_list_node *task = s->task_list.next;

            gyros_list_remove(&SEC_TASK(task)->main_list);
            gyros_list_remove(task);
            gyros__add_task_to_running(SEC_TASK(task));
            gyros__reschedule();
        }
    }
    gyros_interrupt_restore(flags);
}
