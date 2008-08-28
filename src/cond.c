#include <gyros/cond.h>

#include <gyros/sleep.h>

#include "private.h"

#define COND_TASK(t) GYROS_LIST_ENTRY(t, gyros_task_t, cond_list)

static void
add_task_to_cond_list(gyros_cond_t *c, gyros_task_t *task)
{
	struct gyros_list_node *i;

	GYROS_LIST_FOR_EACH(i, &c->task_list)
	{
		if (task->priority > COND_TASK(i)->priority)
		{
			gyros_list_insert(&task->cond_list, i->prev, i);
			return;
		}
	}

	gyros_list_add_last(&task->cond_list, &c->task_list);
}

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
	add_task_to_cond_list(c, gyros__current_task);
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
	add_task_to_cond_list(c, gyros__current_task);
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

        gyros_list_remove(task);
        gyros_list_remove(&COND_TASK(task)->cond_list);
		gyros__add_task_to_running(COND_TASK(task));
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

			gyros_list_remove(task);
            gyros_list_remove(&COND_TASK(task)->cond_list);
            gyros__add_task_to_running(COND_TASK(task));
		}
		gyros__reschedule();
	}
    gyros_interrupt_restore(flags);
}
