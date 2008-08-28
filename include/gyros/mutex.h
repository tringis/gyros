#ifndef INCLUDED__gyros_mutex_h__200808272211
#define INCLUDED__gyros_mutex_h__200808272211

#include <gyros/task.h>

typedef struct gyros_mutex
{
    gyros_task_t *owner;
    struct gyros_list_node task_list;
} gyros_mutex_t;

void gyros_mutex_init(gyros_mutex_t *m);

int gyros_mutex_trylock(gyros_mutex_t *m);

void gyros_mutex_lock(gyros_mutex_t *m);

void gyros_mutex_unlock(gyros_mutex_t *m);

#endif
