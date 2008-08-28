#include <gyros/task.h>

#include <string.h>

#include <gyros/target/interrupt.h>

#include "private.h"

struct gyros_list_node gyros__running = { &gyros__running, &gyros__running };
gyros_task_t *gyros__current_task;

static gyros_task_t s_idle_task;

static int s_next_task_id;

void
gyros__add_task_to_main_list(struct gyros_list_node *list, gyros_task_t *task)
{
    struct gyros_list_node *i;

    for (i = list->next; i != list; i = i->next)
    {
        if (task->priority > MAIN_TASK(i)->priority)
            break;
    }
    gyros_list_insert_before(&task->main_list, i);
}

void
gyros__add_task_to_sec_list(struct gyros_list_node *list, gyros_task_t *task)
{
    struct gyros_list_node *i;

    for (i = list->next; i != list; i = i->next)
    {
        if (task->priority > SEC_TASK(i)->priority)
            break;
    }
    gyros_list_insert_before(&task->sec_list, i);
}

void
gyros__add_task_to_running(gyros_task_t *task)
{
    struct gyros_list_node *i;

    for (i = gyros__running.next; i != &gyros__running; i = i->next)
    {
        if (task->priority > MAIN_TASK(i)->priority)
            break;
    }
    gyros_list_insert_before(&task->main_list, i);
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
    unsigned long flags;

    memset(task, 0, sizeof(gyros_task_t));

    task->id = s_next_task_id++;
    task->priority = priority;

    /* Perform architecture specific initalization */
    gyros_target_task_init(task, entry, arg, stack, stack_size);

    flags = gyros_interrupt_disable();
    gyros__add_task_to_running(task);
    GYROS_LIST_NODE_INIT(&task->sec_list);
    gyros_interrupt_restore(flags);
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

    gyros_list_remove(&gyros__current_task->main_list);
    gyros__add_task_to_running(gyros__current_task);
    gyros_interrupt_restore(flags);

    gyros__reschedule();
}
