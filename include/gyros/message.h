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
#ifndef INCLUDED__gyros_message_h__200902071148
#define INCLUDED__gyros_message_h__200902071148

/** \defgroup message_group Message passing
  *
  * TBD
  */
/*@{*/

/** \file
 * \brief Message passing.
  * \copydoc message_group
 */

#include <gyros/target/config.h>
#include <gyros/private/debug.h>
#include <gyros/private/list.h>
#include <gyros/task.h>
#include <gyros/time.h>

/** \brief Message header struct.
  *
  * This struct must be the first member of any struct representing a
  * message.  See the following example:
\code
struct my_message
{
    gyros_msghdr_t hdr;
    int my_data;
};
\endcode
  */
typedef struct
{
    struct gyros__list_node list; /**< \internal */

    /** Pointer to the sender task, or @c NULL if the message was sent
      * from interrupt context. */
    gyros_task_t *sender;
} gyros_msghdr_t;

/** Send the message @a msg to @a task.
  *
  * \param task         Task to which the message is sent.
  * \param msg          Message to send.
  */
void gyros_send(gyros_task_t *task, void *msg);

/** Receive the next message from the current task's message queue.
  *
  * \return             Address of message.  Never NULL.
  */
void *gyros_receive(void);

/** Receive the next message from the current task's message queue, or
  * return @a NULL if @a timeout has passed.
  *
  * \param timeout      Timeout in absolute time.  See gyros_time().
  * \return             Address of message, or @c NULL if @a timeout
  *                     was reached before a message was received.
  */
void *gyros_receive_until(gyros_abstime_t timeout);

/*@}*/

#endif
