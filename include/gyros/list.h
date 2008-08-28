#ifndef INCLUDED__gyros_list_h__200206071350
#define INCLUDED__gyros_list_h__200206071350

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
gyros_list_remove(struct gyros_list_node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}

#endif
