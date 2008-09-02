#ifndef INCLUDED__gyros_task_h__200206071335
#define INCLUDED__gyros_task_h__200206071335

#include <gyros/list.h>
#include <gyros/target/interrupt.h>
#include <gyros/target/task_regs.h>

typedef struct
{
    struct gyros_task_regs regs;
    struct gyros_list_node main_list;

    struct gyros_list_node timeout_list;
    unsigned long timeout;
    short timed_out;

    struct gyros_list_node task_list;

    int priority;

    const char *name;
    void *stack;
    int stack_size;
} gyros_task_t;

void gyros_init(void);

void gyros_start(void) __attribute__((__noreturn__));

void gyros_task_create(gyros_task_t *task,
                       const char *name,
                       void (*entry)(void *arg),
                       void *arg,
                       void *stack,
                       int stack_size,
                       int priority);

void gyros_task_delete(gyros_task_t *task);

gyros_task_t *gyros_task_current(void);

void gyros_task_suspend(gyros_task_t *task);

void gyros_task_resume(gyros_task_t *task);

void gyros_yield(void);

void gyros_lock(void);

void gyros_unlock(void);

gyros_task_t *gyros_task_iterate(gyros_task_t *previous);

int gyros_task_stack_used(gyros_task_t *task);

static __inline__ unsigned long
gyros_time(void)
{
    extern unsigned long gyros__ticks;

    return gyros__ticks;
}

int gyros_sleep(unsigned long ticks);

int gyros_sleep_until(unsigned long ticks);

#endif
