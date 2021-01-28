/**************************************************************************
 * Copyright (c) 2002-2021, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDED__gyros_smq_h__202101281815
#define INCLUDED__gyros_smq_h__202101281815

/**
  * \file
  * \brief Static Message queues.
  * \details Header file for \ref smq_group.
  *
  * \defgroup smq_group Static message queues
  *
  * A static message queue is used to send small messages between
  * tasks.  The contents of the messages is application defined.
  * Messages are sent to the queue using gyros_smq_send() and received
  * from the queue using gyros_smq_receive() or
  * gyros_smq_receive_until().
  *
  * The message queue must be initialized before use using
  * gyros_smq_init().
  *
  * Message data is copied both when sendind and receiving, which is
  * why the messages should be kept small.
  *
  * @{
  */

#include <limits.h>

#include <gyros/config.h>
#include <gyros/private/debug.h>
#include <gyros/task.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  define GYROS_SMQ_DEBUG_MAGIC           ((unsigned)0x20e21e6f)
#endif

/** \brief Static message queue (smq) object. */
typedef struct
{
    struct gyros_debug_info debug_info; /**< Debug info. */

    struct gyros__list_node task_list; /**< \internal */

    void *memory; /**< \internal */
    unsigned msg_size; /**< \internal */
    unsigned total; /**< \internal */
    unsigned used; /**< \internal */
    unsigned read_pos; /**< \internal */
    unsigned write_pos; /**< \internal */
} gyros_smq_t;

/** Initialize the static message queue @a smq.
  *
  * \param smq          Static message queue struct pointer.
  * \param memory       Message storage.
  * \param msg_size     Message size in bytes.
  * \param msg_count    Number of available messages.
  */
void gyros_smq_init(gyros_smq_t *smq,
                    void *memory,
                    unsigned msg_size,
                    unsigned msg_count);

/** Send the message @a msg to the static message queue @a smq.  May
  * be called from interrupt context.
  *
  * \param smq          Message queue to which the message is sent.
  * \param msg          Message to send.
  * \param block        Non-zero to block if there is no room in the queue.
  * \return             Non-zero if message was sent, else zero.
  */
int gyros_smq_send(gyros_smq_t *smq,
                   const void *msg,
                   int block);

/** Receive the next message from the message queue @a mq.  May be
  * called from interrupt context.
  *
  * \param smq          Message queue from which the message is received.
  * \param msg          Message to send.
  * \return             Non-zero if a message was received, else zero.
  */
int gyros_smq_receive(gyros_smq_t *smq, void *msg);

/** Receive the next message from the message queue @a mq, or return
  * @a NULL if @a timeout has passed.  May not be called from
  * interrupt context.
  *
  * \param mq           Message queue from which the message is received.
  * \param timeout      Timeout in absolute time.  See gyros_time().
  * \return             Non-zero if a message was received, else zero.
  */
int gyros_smq_receive_until(gyros_smq_t *smq,
                            void *msg,
                            gyros_abstime_t timeout);

/** Test if the message queue is empty. May be called from interrupt
  * context.
  *
  * \param mq           Message queue from which the message is received.
  * \return             Non-zero if @a mq was empty, else zero.
  */
int gyros_smq_empty(gyros_smq_t *smq);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
