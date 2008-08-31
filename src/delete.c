#include <gyros/task.h>

#include "private.h"

void
gyros_task_delete(gyros_task_t *task)
{
    gyros_list_remove(&task->main_list);
    gyros_list_remove(&task->timeout_list);

    gyros__cond_reschedule();
}
