#ifndef INCLUDED__gyros_task_h__200206071335
#define INCLUDED__gyros_task_h__200206071335

#include <gyros/list.h>
#include <gyros/target/interrupt.h>
#include <gyros/target/task_regs.h>

typedef struct
{
    struct gyros_task_regs regs;

    struct gyros_list_node list;
    struct gyros_list_node timeout_list;

    int id;
    int priority;

    unsigned long wakeup;
} gyros_task_t;

void gyros_init(void);

void gyros_start(void) __attribute__((__noreturn__));

void gyros_task_create(gyros_task_t *task,
                       void (*entry)(void *arg),
                       void *arg,
                       void *stack,
                       int stack_size,
                       int priority);

int  gyros_task_id(void);

void gyros_yield(void);

static __inline__ unsigned long
gyros_time(void)
{
    extern unsigned long gyros__ticks;

    return gyros__ticks;
}

void gyros_sleep(unsigned long ticks);

void gyros_sleep_until(unsigned long ticks);

#endif
