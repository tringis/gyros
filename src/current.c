#include "private.h"

gyros_task_t*
gyros_task_current(void)
{
    return gyros__current_task;
}
