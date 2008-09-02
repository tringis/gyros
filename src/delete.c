#include <gyros/task.h>

#include "private.h"

void
gyros_task_delete(gyros_task_t *task)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros_list_remove(&task->main_list);
    gyros_list_remove(&task->timeout_list);
    gyros_list_remove(&gyros__state.current->task_list);
    gyros__cond_reschedule();
    gyros_interrupt_restore(flags);
}
