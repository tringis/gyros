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
#ifndef INCLUDED__gyros_private_debug_h__200812022021
#define INCLUDED__gyros_private_debug_h__200812022021

#include <gyros/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Structure containing debug information on an object. This struct
  * is the first member of all synchronization objects. */
struct gyros_debug_info
{
#if GYROS_CONFIG_DEBUG
    unsigned magic; /**< \internal */
    const char *name; /**< Pointer to name of object, or @c NULL. */
#endif
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  if GYROS_CONFIG_DEBUG
#    define GYROS_DEBUG_INFO(magic, name)     { (magic), (name) }
#    define GYROS_DEBUG_INFO_INIT(obj, obj_magic)                          \
            do {                                                           \
                (obj)->debug_info.magic = (obj_magic);                     \
                (obj)->debug_info.name = NULL;                             \
            } while (0)
#    define GYROS_DEBUG_SET_STATE(task, state)                             \
            do {                                                           \
                (task)->debug_state = (state);                             \
                (task)->debug_object = (const struct gyros_debug_info*)0;  \
            } while (0)
#    define GYROS_DEBUG_SET_STATE2(task, state, object)                    \
            do {                                                           \
                (task)->debug_state = (state);                             \
                (task)->debug_object = &(object)->debug_info;              \
            } while (0)
#  else
#    define GYROS_DEBUG_INFO(magic, name)                { }
#    define GYROS_DEBUG_INFO_INIT(obj, obj_magic)        do { } while (0)
#    define GYROS_DEBUG_SET_STATE(task, state)           do { } while (0)
#    define GYROS_DEBUG_SET_STATE2(task, state, object)  do { } while (0)
#  endif
#endif

#ifdef __cplusplus
}
#endif

#endif
