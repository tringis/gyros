#ifndef INCLUDED__gyros_sem_h__200808281430
#define INCLUDED__gyros_sem_h__200808281430

#include <gyros/task.h>

typedef struct gyros_sem
{
    unsigned value;
    unsigned max_value;
    struct gyros_list_node task_list;
} gyros_sem_t;

void gyros_sem_init(gyros_sem_t *s, int start_value);

void gyros_sem_init_binary(gyros_sem_t *s);

void gyros_sem_wait(gyros_sem_t *s);

int gyros_sem_timedwait(gyros_sem_t *s, int timeout);

void gyros_sem_signal(gyros_sem_t *s);

#endif
