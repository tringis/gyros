/**************************************************************************
 * Copyright (c) 2002-2008, Tobias Ringström <tobias@ringis.se>
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
#ifndef INCLUDED__gyros_list_h__200206071350
#define INCLUDED__gyros_list_h__200206071350

/** \internal */
struct gyros_list_node {
    struct gyros_list_node *next, *prev;
};

#define GYROS_LIST_NODE_INIT(n) do { (n)->prev=(n); (n)->next=(n); } while (0)

#define GYROS_LIST_CONTAINER(node, type, member) \
    ((type*)((char*)(node)-(unsigned long)(&((type*)0)->member)))

static __inline int
gyros_list_empty(struct gyros_list_node *list)
{
    return list->next == list;
}

static __inline void
gyros_list_insert_before(struct gyros_list_node *new_node,
                         struct gyros_list_node *list_node)
{
    struct gyros_list_node *prev_node = list_node->prev;

    new_node->next = list_node;
    new_node->prev = prev_node;
    list_node->prev = new_node;
    prev_node->next = new_node;
}

static __inline void
gyros_list_remove(struct gyros_list_node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    /* Make the removed node point to itself */
    node->next = node;
    node->prev = node;
}

#endif
