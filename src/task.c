#include <gyros/task.h>

#include <string.h>

#include <gyros/target/interrupt.h>

#include "private.h"

struct gyros_list_node gyros__running = { &gyros__running, &gyros__running };
gyros_task_t *gyros__current_task;

static gyros_task_t s_idle_task;

static int s_next_task_id;

void
gyros__add_task_to_list(struct gyros_list_node *list, gyros_task_t *task)
{
	struct gyros_list_node *i;

	GYROS_LIST_FOR_EACH(i, list)
	{
		if (task->priority > TASK(i)->priority)
		{
			gyros_list_insert(&task->main_list, i->prev, i);
			return;
		}
	}

	gyros_list_add_last(&task->main_list, list);
}

void
gyros__add_task_to_running(gyros_task_t *task)
{
    gyros__add_task_to_list(&gyros__running, task);
}

void
gyros_target_task_init(gyros_task_t *task,
                       void (*entry)(void *arg),
                       void *arg,
                       void *stack,
                       int stack_size);
void
gyros_task_create(gyros_task_t *task,
                  void (*entry)(void *arg),
                  void *arg,
                  void *stack,
                  int stack_size,
                  int priority)
{
	memset(task, 0, sizeof(gyros_task_t));

	task->id = s_next_task_id++;
	task->priority = priority;

	/* Perform architecture specific initalization */
	gyros_target_task_init(task, entry, arg, stack, stack_size);

	gyros_interrupts_disable();
	gyros__add_task_to_running(task);
	GYROS_INIT_LIST_NODE(&task->cond_list);
	gyros_interrupts_enable();
}

void
gyros_init(void)
{
    /* Make the current "task" the idle task */
    s_next_task_id = 0;
    s_idle_task.id = s_next_task_id;
	s_idle_task.priority = 0;

	gyros__add_task_to_running(&s_idle_task);
    gyros__current_task = &s_idle_task;

    gyros__tick_enable();

    gyros_interrupts_enable();
}

void
gyros_yield(void)
{
    gyros_interrupts_disable();
    gyros_list_remove(&gyros__current_task->main_list);
	gyros__add_task_to_running(gyros__current_task);
    gyros_interrupts_enable();

    gyros__reschedule();
}
