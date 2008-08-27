#include <string.h>

#include <gyros/target/interrupt.h>
#include <gyros/task.h>
/* #include <gyros/time.h> */
/* #include <gyros/compiler.h> */

#define TASK(t) GYROS_LIST_ENTRY(t, gyros_task_t, main_list)

struct gyros_list_node gyros_running;
static int s_next_task_id;

static gyros_task_t s_idle_task;

gyros_task_t *gyros_current_task;
//int gyros_need_reschedule = 0;

static void
add_task_to_running(gyros_task_t *task)
{
	struct gyros_list_node *i;

	GYROS_LIST_FOR_EACH(i, &gyros_running)
	{
		if (task->priority > TASK(i)->priority)
		{
			gyros_list_insert(&task->main_list, i->prev, i);
			return;
		}
	}

	gyros_list_add_last(&task->main_list, &gyros_running);
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
	add_task_to_running(task);
	gyros_interrupts_enable();
}

void
gyros_init(void)
{
	GYROS_INIT_LIST_NODE(&gyros_running);

    /* Make the current "task" the idle task */
    s_next_task_id = 0;
    s_idle_task.id = s_next_task_id;
	s_idle_task.priority = 0;

    gyros_current_task = &s_idle_task;

	add_task_to_running(&s_idle_task);
}

void
gyros__yield(void)
{
    gyros_list_remove(&gyros_current_task->main_list);
	add_task_to_running(gyros_current_task);
}
