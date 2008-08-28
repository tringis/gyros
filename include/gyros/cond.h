#ifndef INCLUDED__gyros_cond_h__200808272231
#define INCLUDED__gyros_cond_h__200808272231

#include <gyros/mutex.h>

typedef struct gyros_cond {
    struct gyros_list_node task_list;
} gyros_cond_t;

void gyros_cond_init(gyros_cond_t *c);

void gyros_cond_wait(gyros_cond_t *c, gyros_mutex_t *m);

void gyros_cond_timedwait(gyros_cond_t *c, gyros_mutex_t *m, int timeout);

void gyros_cond_signal_one(gyros_cond_t *c);

void gyros_cond_signal_all(gyros_cond_t *c);

#endif
