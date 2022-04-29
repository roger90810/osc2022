#ifndef _LIST_H
#define _LIST_H


struct list_head {
    struct list_head *next, *prev;
};

struct list_node {
    struct list_node *next;
};

/**
 * INIT_LIST_HEAD - Initialize a list_head structure
 * @list: list_head structure to be initialized.
 *
 * Initializes the list_head to point to itself.  If it is a list header,
 * the result is an empty list.
 */
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

#endif