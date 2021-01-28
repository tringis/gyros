/**************************************************************************
 * Copyright (c) 2002-2009, Tobias Ringström <tobias@ringis.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/
#ifndef INCLUDED__gyros_trace_h__200905062204
#define INCLUDED__gyros_trace_h__200905062204

#include <gyros/cond.h>
#include <gyros/event_group.h>
#include <gyros/mq.h>
#include <gyros/mutex.h>
#include <gyros/rwlock.h>
#include <gyros/sem.h>
#include <gyros/smq.h>
#include <gyros/task.h>
#include <gyros/timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \file
  * \brief Trace support.
  * \details Header file for \ref trace_group.
  *
  * \defgroup trace_group Tracing support
  *
  * Tracing is enabled by setting \ref GYROS_CONFIG_TRACE to a
  * non-zero value in gyros/config.h.  When tracing is enabled,
  * task activities such as context switches and synchronization
  * object interactions, e.g. locking and unlocking of mutexes, are
  * logged with time stamps.  These log entries can be recalled to
  * study the real time behavior of a program.
  *
  * Before tracing can be enabled, a memory area must be reserved to
  * store the trace log.  The address and size of the memory area is
  * supplied when calling gyros_trace_init().  Tracing is enabled and
  * disabled by the running software.  When disabling tracing, there
  * is an option to perform a number of additional traces before
  * really stopping, which can be useful to study events up to and
  * just after a certain condition occurs.
  *
  * @{
  */

/** Type a single trace log entry. */
enum gyros_trace_kind
{
    GYROS_TRACE_EMPTY,              /**< Empty trace */
    GYROS_TRACE_TRACE,              /**< Trace on/off */
    GYROS_TRACE_STRING,             /**< String */
    GYROS_TRACE_RUNNING,            /**< Running task */
    GYROS_TRACE_IRQ,                /**< Interrupt */
    GYROS_TRACE_CONTEXT,            /**< Context switch */
    GYROS_TRACE_WAKE,               /**< Task wakeup */
    GYROS_TRACE_TIMER_START,        /**< gyros_timer_start() */
    GYROS_TRACE_TIMER_STOP,         /**< gyros_timer_stop() */
    GYROS_TRACE_TIMER_CALLBACK,     /**< Timer callback. */
    GYROS_TRACE_COND_WAIT,          /**< gyros_cond_wait() blocking */
    GYROS_TRACE_COND_SIGNAL_ONE,    /**< gyros_cond_signal_one() */
    GYROS_TRACE_COND_SIGNAL_ALL,    /**< gyros_cond_signal_all() */
    GYROS_TRACE_MQ_SEND,            /**< gyros_mq_send() */
    GYROS_TRACE_MQ_RECEIVE_BLOCKED, /**< gyros_mq_receive() blocking */
    GYROS_TRACE_MQ_RECEIVED,        /**< gyros_mq_receive() returning */
    GYROS_TRACE_SMQ_SEND,           /**< gyros_smq_send() */
    GYROS_TRACE_SMQ_RECEIVE_BLOCKED,/**< gyros_smq_receive() blocking */
    GYROS_TRACE_SMQ_RECEIVED,       /**< gyros_smq_receive() returning */
    GYROS_TRACE_MUTEX_BLOCKED,      /**< gyros_mutex_lock() blocking */
    GYROS_TRACE_MUTEX_AQUIRED,      /**< gyros_mutex_lock() aquired mutex */
    GYROS_TRACE_MUTEX_UNLOCK,       /**< gyros_mutex_unlock() */
    GYROS_TRACE_RWLOCK_RD_BLOCKED,  /**< gyros_rwlock_rdlock() blocking */
    GYROS_TRACE_RWLOCK_RD_AQUIRED,  /**< gyros_rwlock_rdlock() aquired lock */
    GYROS_TRACE_RWLOCK_RD_UNLOCK,   /**< gyros_rwlock_unlock()
                                         unlocked read lock */
    GYROS_TRACE_RWLOCK_WR_BLOCKED,  /**< gyros_rwlock_wrlock() blocking */
    GYROS_TRACE_RWLOCK_WR_AQUIRED,  /**< gyros_rwlock_wrlock() aquired lock */
    GYROS_TRACE_RWLOCK_WR_UNLOCK,   /**< gyros_rwlock_unlock()
                                         unlocked write lock */
    GYROS_TRACE_SEM_BLOCKED,        /**< gyros_sem_wait() blocking */
    GYROS_TRACE_SEM_AQUIRED,        /**< gyros_sem_wait() aquired semaphore */
    GYROS_TRACE_SEM_SIGNAL,         /**< gyros_sem_signal() */

    GYROS_TRACE_EVENT_GROUP_BLOCKED, /**< gyros_event_group_get() blocking */
    GYROS_TRACE_EVENT_GROUP_AQUIRED, /**< gyros_event_group_get() got event */
    GYROS_TRACE_EVENT_GROUP_SET,     /**< gyros_event_group_set() */
    GYROS_TRACE_EVENT_GROUP_CLEAR,   /**< gyros_event_group_clear() */

    GYROS__TRACE_KIND_COUNT         /**< \internal */
};

/** \brief Trace log entry. */
typedef struct
{
    /** Type of trace. */
    enum gyros_trace_kind kind;

    /** Timestamp of trace */
    gyros_abstime_t timestamp;

    /** Task causing the trace, or @c NULL if the trace was caused
      * from interrupt context. */
    gyros_task_t *task;

    /** \brief Extra information available for some trace kinds. */
    union info
    {
        /** For @c GYROS_TRACE_TRACE: New trace setting (1 = on, 0 =
          * off, negative value = counter to off). */
        int trace;
        /** For @c GYROS_TRACE_STRING: String pointer. */
        const char *str;
        /** For @c GYROS_TRACE_TASK: Task pointer. */
        const gyros_task_t *running;
        /** For @c GYROS_TRACE_CONTEXT: Next task to run. */
        gyros_task_t *context_next;
        /** For @c GYROS_TRACE_WAKEUP: Next task to run. */
        gyros_task_t *wake_task;
#ifdef GYROS_CONFIG_TIMER
        /** For @c GYROS_TRACE_TIMER_START: Timer information. */
        struct gyros_trace_timer_start_info
        {
            /** The timer. */
            gyros_timer_t *timer;
            /** Timer timeout. */
            gyros_abstime_t timeout;
            /** Timer period. */
            gyros_reltime_t period;
        } timer_start;
        /** For @c GYROS_TRACE_TIMER_STOP and @c
         * GYROS_TRACE_TIMER_CALLBACK: The timer. */
        gyros_timer_t *timer;
#endif
        /** For @c GYROS_TRACE_COND_*: The condition variable. */
        gyros_cond_t *cond;
        /** For @c GYROS_TRACE_MQ_*: The message queue. */
        gyros_mq_t *mq;
        /** For @c GYROS_TRACE_SMQ_*: The static message queue. */
        gyros_smq_t *smq;
        /** For @c GYROS_TRACE_MUTEX_*: The mutex. */
        gyros_mutex_t *mutex;
        /** For @c GYROS_TRACE_RWLOCK_*: The read/write lock. */
        gyros_rwlock_t *rwlock;
        /** For @c GYROS_TRACE_SEM_*: Semaphore information. */
        struct gyros_trace_sem_info
        {
            /** The semaphore. */
            gyros_sem_t *sem;
            /** Semaphore value. */
            unsigned value;
        } sem;
        struct gyros_trace_event_group_info
        {
            /** The event group. */
            gyros_event_group_t *event_group;
            /** Event group events. */
            unsigned events;
        } event_group;
    } info;
} gyros_trace_t;

/** Initialize tracing.  Must be called before any other trace
  * functions.  Does not clear the trace log to allow for iterating
  * over a previous trace log.  Use gyros_trace_clear() to clear
  * the trace log.
  *
  * \param log              Address of log area.  Must be int aligned.
  * \param log_size         Size of log area in bytes.
  */
void gyros_trace_init(void *log, int log_size);

/** Clear the trace log.  Must not be called when tracing is enabled.
  */
void gyros_trace_clear(void);

/** Set the current trace position.
  *
  * \param pos              New position.  Must be a valid position
  *                         returned by gyros_trace_iterate().
  */
void gyros_trace_seek(gyros_trace_t *pos);

/** Enable tracing.
  */
void gyros_trace_on(void);

/** Disable tracing, either right away if @a when is zero, or after @a
  * when numner of traces.
  *
  * \param when             Number of additional traces to log.
  */
void gyros_trace_off(int when);

/** Insert a custom string in the trace log.  Note that the pointer to
  * the string is stored, so the pointer must remain valid even after
  * this call.
  *
  * \param str              String to add to the trace log.
  */
void gyros_trace_string(const char *str);

/** Insert a trace of all currently running tasks into the trace log.
  */
void gyros_trace_running_tasks(void);

/** Iterate through the trace log.  The first call must be with @a
  * prev set to @c NULL, and subsequent calls with the return value of
  * the previous call until @c NULL is returned.
  *
  * \param prev             Pointer to previous trace.
  * \return                 Pointer to next trace, or @c NULL if there
  *                         are no more traces.
  */
gyros_trace_t *gyros_trace_iterate(gyros_trace_t *prev);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
