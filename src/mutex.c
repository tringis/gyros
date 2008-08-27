#include <gyros/mutex.h>

#include <stdlib.h>

#include "private.h"

void
gyros_mutex_init(gyros_mutex_t *m)
{
	m->owner = NULL;
	GYROS_INIT_LIST_NODE(&m->task_list);
}

int
gyros_mutex_trylock(gyros_mutex_t *m)
{
    int ok;

    gyros_interrupts_disable();
	if (m->owner)
		ok = 0;
    else
        m->owner = gyros__current_task;
    gyros_interrupts_enable();

	return ok;
}

void
gyros_mutex_lock(gyros_mutex_t *m)
{
    gyros_interrupts_disable();
	while (m->owner)
	{
		gyros_list_remove(&gyros__current_task->main_list);
		gyros__add_task_to_list(&m->task_list, gyros__current_task);
		gyros__reschedule();
	}

	m->owner = gyros__current_task;
    gyros_interrupts_enable();
}

void
gyros__mutex_unlock(gyros_mutex_t *m, int reschedule)
{
    gyros_interrupts_disable();
	m->owner = NULL;
	if (!gyros_list_empty(&m->task_list))
	{
		struct gyros_list_node *task = m->task_list.next;

		gyros_list_remove(task);
		gyros__add_task_to_running(TASK(task));
		if (reschedule)
			gyros__reschedule();
	}
}

void
gyros_mutex_unlock(gyros_mutex_t *m)
{
	gyros__mutex_unlock(m, 1);
}
