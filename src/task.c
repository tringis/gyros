#include <gyros/task.h>

#include <string.h>

#include <gyros/target/interrupt.h>

#include "private.h"

gyros__state_t gyros__state;
struct gyros_list_node gyros__tasks = { &gyros__tasks, &gyros__tasks };
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
    gyros_list_insert_before(&task->main_list, i);
}

void
gyros__task_exit(void)
{
    /* Note that we do not need to call gyros_interrupt_restore()
     * because gyros__reschedule below never returns. */
    gyros_interrupt_disable();
    gyros_list_remove(&gyros__state.current->main_list);
    gyros_list_remove(&gyros__state.current->task_list);
    gyros__reschedule();
}

void
gyros__task_move(gyros_task_t *task, struct gyros_list_node *list)
{
    gyros_list_remove(&task->main_list);
    add_task_to_list(task, list);
}

void
gyros__task_wake(gyros_task_t *task)
{
    gyros_list_remove(&task->main_list);
    gyros_list_remove(&task->timeout_list);

    add_task_to_list(task, &gyros__state.running);
}

void
gyros_init(void)
{
    GYROS_LIST_NODE_INIT(&gyros__state.running);

    gyros__target_init();

    /* Make the current "task" the idle task */
    s_idle_task.priority = 0;

    s_idle_task.name = "[idle]";
    s_idle_task.stack = 0;
    s_idle_task.stack_size = 0;

    gyros_list_insert_before(&s_idle_task.task_list, &gyros__tasks);
    add_task_to_list(&s_idle_task, &gyros__state.running);
    gyros__state.current = &s_idle_task;
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
gyros_task_create(gyros_task_t *task,
                  const char *name,
                  void (*entry)(void *arg),
                  void *arg,
                  void *stack,
                  int stack_size,
                  int priority)
{
    unsigned long flags;

    memset(task, 0, sizeof(gyros_task_t));

    task->priority = priority;

    task->name = name;
    task->stack = stack;
    task->stack_size = stack_size;

    /* Perform architecture specific initalization */
    gyros__target_task_init(task, entry, arg, stack, stack_size);

    flags = gyros_interrupt_disable();
    gyros_list_insert_before(&task->task_list, &gyros__tasks);
    add_task_to_list(task, &gyros__state.running);
    GYROS_LIST_NODE_INIT(&task->timeout_list);
    gyros_interrupt_restore(flags);
}

void
gyros_yield(void)
{
    unsigned long flags = gyros_interrupt_disable();

    gyros_list_remove(&gyros__state.current->main_list);
    add_task_to_list(gyros__state.current, &gyros__state.running);
    gyros__cond_reschedule();
    gyros_interrupt_restore(flags);
}
