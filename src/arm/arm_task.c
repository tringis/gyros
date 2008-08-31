#include <gyros/task.h>

#include <gyros/arm/arm_defs.h>

#include "../private.h"


void
gyros__target_task_init(gyros_task_t *task,
                        void (*entry)(void *arg),
                        void *arg,
                        void *stack,
                        int stack_size)
{
    task->regs.r[0] = (uint32_t)arg;
    task->regs.sp = (uint32_t)stack + stack_size;
    task->regs.lr = (uint32_t)gyros__task_exit;
    task->regs.pc = (uint32_t)entry + 4;
    task->regs.psr = ARM_MODE_SYS;
}
