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
#ifndef INCLUDED__gyros_mq_h__200902071051
#define INCLUDED__gyros_mq_h__200902071051

/**
  * \file
  * \brief Message queues.
  * \details Header file for \ref mq_group.
  *
  * \defgroup mq_group Message queues
  *
  * A message queue is used to send messages between tasks.  The
  * contents of the messages is application defined.  Messages are
  * sent to the queue using gyros_mq_send() and received from the
  * queue using gyros_mq_receive() or gyros_mq_receive_until().
  *
  * The message queue must be initialized before use, either using
  * GYROS_MQ_INITVAL() when defining the message queue, or using
  * gyros_mq_init().
  *
  * Message data types are structs whose first member is an instance
  * of gyros_mq_msghdr_t.  See the documentation of \ref
  * gyros_mq_msghdr_t for an example of a message data type.
  *
  * It is convenient to use a \ref mempool_group "memory pool" for
  * message allocation:
  * 
  * \code
  * struct my_message
  * {
  *     gyros_mq_msghdr_t hdr;
  *     int my_data;
  * };
  *
  * void send(int data)
  * {
  *     struct my_message *msg = gyros_alloc(zpool, sizeof(*msg));
  *     msg->my_data = data;
  *     gyros_mq_send(&mq, msg);
  * }
  *
  * void receive(void)
  * {
  *     struct my_message *msg = gyros_mq_receive(&mq);
  *     printf("data = %d\n", msg->my_data);
  *     gyros_free(msg);
  * }
  * \endcode
  *
  * @{
  */

#include <limits.h>
#include <stdbool.h>

#include <gyros-config.h>
#include <gyros/private/debug.h>
#include <gyros/task.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_MQ_DEBUG_MAGIC                 ((unsigned)0x736a0997)
#endif

/** \brief Message header struct.
  *
  * This struct must be the first member of any struct representing a
  * message to be sent by gyros_mq_send().  See the following example:
  *
  * \code
  * struct my_message
  * {
  *     gyros_mq_msghdr_t hdr;
  *     int my_data;
  * };
  * \endcode
  */
typedef struct
{
    struct gyros__list_node list; /**< \internal */
} gyros_mq_msghdr_t;

/** Initialization value for a message queue by the specified @a name.
  * When a message queue is initialized using this value,
  * gyros_mq_init() does not need to be called.  Example:
  *
  * \code
  * gyros_mq_t my_mq = GYROS_MQ_INITVAL(my_mq);
  * \endcode
  *
  * \param name         Name of the message queue variable.
  */
#define GYROS_MQ_INITVAL(name) \
    { GYROS_DEBUG_INFO(GYROS_MQ_DEBUG_MAGIC, #name),                    \
      GYROS__LIST_INITVAL((name).msg_list),                             \
      GYROS__LIST_INITVAL((name).task_list) }

/** \brief Message queue (mq) object. */
typedef struct
{
    struct gyros_debug_info debug_info; /**< Debug info. */

    struct gyros__list_node msg_list; /**< \internal */
    struct gyros__list_node task_list; /**< \internal */
} gyros_mq_t;

/** Initialize the message queue @a mq.
  *
  * \param mq           Message queue struct pointer.
  */
void gyros_mq_init(gyros_mq_t *mq);

/** Send the message @a msg to the message queue @a mq.  May be called
  * from interrupt context.
  *
  * \param mq           Message queue to which the message is sent.
  * \param msg          Message to send.
  */
void gyros_mq_send(gyros_mq_t *mq, void *msg);

/** Receive the next message from the message queue @a mq.  May be
  * called from interrupt context.
  *
  * \param mq           Message queue from which the message is received.
  * \return             Address of message, or NULL if called from
  *                     interrupt context and no message was available.
  */
void *gyros_mq_receive(gyros_mq_t *mq);

/** Receive the next message from the message queue @a mq, or return
  * @a NULL if @a timeout has passed.  May not be called from
  * interrupt context.
  *
  * \param mq           Message queue from which the message is received.
  * \param timeout      Timeout in absolute time.  See gyros_time().
  * \return             Address of message, or @c NULL if @a timeout
  *                     was reached before a message was received.
  */
void *gyros_mq_receive_until(gyros_mq_t *mq, gyros_abstime_t timeout);

/** Test if the message queue is empty. May be called from interrupt
  * context.
  *
  * \param mq           Message queue from which the message is received.
  * \return             True if @a mq was empty, else false.
  */
bool gyros_mq_empty(gyros_mq_t *mq);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
