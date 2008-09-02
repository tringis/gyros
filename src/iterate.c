#include "private.h"

#define TASK_LIST_TASK(t) GYROS_LIST_CONTAINER(t, gyros_task_t, task_list)

gyros_task_t*
gyros_task_iterate(gyros_task_t *previous)
{
    if (!previous)
        return TASK_LIST_TASK(gyros__tasks.next);

    if (previous->task_list.next == &gyros__tasks)
        return 0;

    return TASK_LIST_TASK(previous->task_list.next);
}
