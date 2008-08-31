#include <gyros/task.h>

#include <string.h>

#include <gyros/target/interrupt.h>

#include "private.h"

struct gyros_list_node gyros__running = { &gyros__running, &gyros__running };
gyros_task_t *gyros__current_task;

static gyros_task_t s_idle_task;

static void
add_task_to_list(gyros_task_t *task, struct gyros_list_node *list)
{
    struct gyros_list_node *i;

    for (i = list->next; i != list; i = i->next)
    {
        if (task->priority > TASK(i)->priority)
            break;
    }
    gyros_list_insert_before(&task->list, i);
}

void
gyros__task_move(gyros_task_t *task, struct gyros_list_node *list)
{
    gyros_list_remove(&task->list);
    add_task_to_list(task, list);
}

void
gyros__task_wake(gyros_task_t *task)
{
    gyros_list_remove(&task->list);
    gyros_list_remove(&task->timeout_list);

    add_task_to_list(task, &gyros__running);
}

void
gyros_task_create(gyros_task_t *task,
                  void (*entry)(void *arg),
                  void *arg,
                  void *stack,
                  int stack_size,
                  int priority)
{
    unsigned long flags;

    memset(task, 0, sizeof(gyros_task_t));

    task->priority = priority;

    /* Perform architecture specific initalization */
    gyros__target_task_init(task, entry, arg, stack, stack_size);

    flags = gyros_interrupt_disable();
    add_task_to_list(task, &gyros__running);
    GYROS_LIST_NODE_INIT(&task->timeout_list);
    gyros_interrupt_restore(flags);
}

void
gyros_init(void)
{
    /* Make the current "task" the idle task */
    s_idle_task.priority = 0;

    add_task_to_list(&s_idle_task, &gyros__running);
    gyros__current_task = &s_idle_task;
}

void
gyros_start(void)
{
    gyros__tick_enable();
    gyros__interrupt_enable();

    for (;;)
        ;
}

void
gyros_yield(void)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros_list_remove(&gyros__current_task->list);
    add_task_to_list(gyros__current_task, &gyros__running);
    gyros_interrupt_restore(flags);

    gyros__reschedule();
}
