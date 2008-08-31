#include <gyros/task.h>

#include <gyros/target/interrupt.h>

#include "private.h"

void
gyros_task_suspend(gyros_task_t *task)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros_list_remove(&task->main_list);
    gyros_list_remove(&task->timeout_list);
    if (task == gyros__current_task)
        gyros__reschedule();
    gyros_interrupt_restore(flags);
}

void
gyros_task_resume(gyros_task_t *task)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros__task_move(task, &gyros__running);
    if (task->priority > gyros__current_task->priority)
        gyros__reschedule();
    gyros_interrupt_restore(flags);
}
