#ifndef INCLUDED__gyros_list_h__200206071350
#define INCLUDED__gyros_list_h__200206071350

struct gyros_list_node {
    struct gyros_list_node *next, *prev;
};

#define GYROS_INIT_LIST_NODE(n) do { (n)->prev=(n); (n)->next=(n); } while (0)

static __inline int
gyros_list_empty(struct gyros_list_node *list)
{
    return list->next == list;
}

static __inline void
gyros_list_insert(struct gyros_list_node *node,
                   struct gyros_list_node *prev,
                   struct gyros_list_node *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static __inline void
gyros_list_add_first(struct gyros_list_node *node,
                      struct gyros_list_node *list)
{
    gyros_list_insert(node, list, list->next);
}

static __inline void
gyros_list_add_last(struct gyros_list_node *node,
                     struct gyros_list_node *list)
{
    gyros_list_insert(node, list->prev, list);
}

static __inline void
gyros_list_move_last(struct gyros_list_node *to,
                      struct gyros_list_node *from)
{
    if (!gyros_list_empty(from))
    {
        from->next->prev = to->prev;
        from->prev->next = to;
        to->prev->next = from->next;
        to->prev = from->prev;
        GYROS_INIT_LIST_NODE(from);
    }
}

static __inline void
gyros_list_remove(struct gyros_list_node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}

#define GYROS_LIST_ENTRY(node, type, member) \
    ((type*)((char*)(node)-(unsigned long)(&((type*)0)->member)))

#define GYROS_LIST_FOR_EACH(pos, head) \
    for ((pos) = (head)->next; (pos) != (head); \
            (pos) = (pos)->next)

#endif
