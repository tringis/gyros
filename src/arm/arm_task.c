#include <string.h>

#include <gyros/arm/arm_defs.h>

#include "../private.h"

void
gyros__target_task_init(gyros_task_t *task,
                        void (*entry)(void *arg),
                        void *arg,
                        void *stack,
                        int stack_size)
{
    uint32_t *p = task->stack;
    uint32_t *e = (uint32_t*)((uint32_t)task->stack + task->stack_size);

    task->regs.r[0] = (uint32_t)arg;
    task->regs.sp = (uint32_t)stack + stack_size;
    task->regs.lr = (uint32_t)gyros__task_exit;
    task->regs.pc = (uint32_t)entry + 4;
    task->regs.psr = ARM_MODE_SYS;

    do
        *p++ = 0xeeeeeeee;
    while (p != e);
}

int
gyros_task_stack_used(gyros_task_t *task)
{
    uint32_t *p = task->stack;
    uint32_t *e = (uint32_t*)((uint32_t)task->stack + task->stack_size);

    while (p != e && *p == 0xeeeeeeee)
        p++;

    return (uint32_t)e - (uint32_t)p;
}
