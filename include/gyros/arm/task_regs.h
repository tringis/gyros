#ifndef INCLUDED__arm_task_h__200212281230
#define INCLUDED__arm_task_h__200212281230

#include <stdint.h>

struct gyros_task_regs {
    uint32_t r[13];
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
};

#endif
