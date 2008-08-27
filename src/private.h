#ifndef INCLUDED__gyros_private_h__200808271854
#define INCLUDED__gyros_private_h__200808271854

#define TASK(t) GYROS_LIST_ENTRY(t, gyros_task_t, main_list)

#include <gyros/task.h>
#include <gyros/mutex.h>

extern gyros_task_t *gyros__current_task;
extern struct gyros_list_node gyros__running;
extern unsigned long gyros__ticks;

void gyros__tick_enable(void);

void gyros__add_task_to_list(struct gyros_list_node *list, gyros_task_t *task);

void gyros__add_task_to_running(gyros_task_t *task);

void gyros__wake_sleeping_tasks(void);

void gyros__mutex_unlock(gyros_mutex_t *m, int reschedule);

#endif
