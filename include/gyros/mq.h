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

/** \defgroup mq_group Message queues
  *
  */
/*@{*/

/** \file
  * \brief Message queues.
  * \copydoc mq_group
 */

#include <limits.h>

#include <gyros/target/config.h>
#include <gyros/private/debug.h>
#include <gyros/task.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#if GYROS_CONFIG_DEBUG
#define GYROS_MQ_DEBUG_MAGIC           ((unsigned)0x736a0997)
#define GYROS_MQ_DEBUG_INITIALIZER     GYROS_MQ_DEBUG_MAGIC,(char*)0,
#else
#define GYROS_MQ_DEBUG_INITIALIZER
#endif
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
  */
#define GYROS_MQ_INITVAL(name) \
    { GYROS_MQ_DEBUG_INITIALIZER                                        \
      GYROS__LIST_INITVAL(name.msg_list),                               \
      GYROS__LIST_INITVAL(name.task_list) }

/** \brief Message queue (mq) object. */
typedef struct
{
#if GYROS_CONFIG_DEBUG
    unsigned debug_magic; /**< \internal */
    /** Name of the message queue set by gyros_debug_name(), else @c
      * NULL. */
    const char *name;
#endif

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

/*@}*/

#endif
