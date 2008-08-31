#ifndef INCLUDED__gyros_private_h__200808271854
#define INCLUDED__gyros_private_h__200808271854

#define TASK(t) GYROS_LIST_CONTAINER(t, gyros_task_t, main_list)

#include <gyros/task.h>
#include <gyros/mutex.h>

extern struct gyros_list_node gyros__running;
extern gyros_task_t *gyros__current_task;
extern unsigned long gyros__ticks;

void gyros__task_move(gyros_task_t *task, struct gyros_list_node *list);

void gyros__task_wake(gyros_task_t *task);

void gyros__task_set_timeout(unsigned long timeout);

void gyros__wake_sleeping_tasks(void);

void gyros__mutex_unlock(gyros_mutex_t *m, int reschedule);

/* The following functions must be implemented by the target */

void gyros__target_task_init(gyros_task_t *task,
                             void (*entry)(void *arg),
                             void *arg,
                             void *stack,
                             int stack_size);

void gyros__tick_enable(void);

#endif
